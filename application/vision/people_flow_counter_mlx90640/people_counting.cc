#include "people_counting.h"
#include "mlx90640/mlx90640.h"
#include "sl_i2cspm_instances.h"
#include "sl_sleeptimer.h"
#include "sl_tflite_micro_model.h"
#include "sl_tflite_micro_init.h"
#include "app_assert.h"
#include "arm_math.h"
#include "bluetooth.h"

#define OUTPUT_OVER_BLE             true

#define OUTPUT_WIDTH                16
#define OUTPUT_HEIGHT               12
#define OUTPUT_DEPTH                6
#define INPUT_OUTPUT_RATIO          MLX90640_WIDTH / OUTPUT_WIDTH
#define MAX_NUM_BOUNDING_BOXES      OUTPUT_WIDTH*OUTPUT_HEIGHT
#define MAX_TRACKING_DIST           20
#define IOU_THRESHOLD               0.3f
#define CONFIDENCE_THRESHOLD        0.55f
#define CROSSING_X                  20

static TfLiteTensor * model_input;
static tflite::MicroInterpreter* interpreter;

static float ir_img_buf[MLX90640_WIDTH * MLX90640_HEIGHT];
static float label_image_buf[OUTPUT_WIDTH * OUTPUT_HEIGHT];

static struct Image raw_img;
static struct Image model_input_img;
static struct Image heatmap_img;
static struct Image label_image;

static struct bbox bboxes[MAX_NUM_BOUNDING_BOXES];
static struct bbox final_bboxes[MAX_NUM_BOUNDING_BOXES / 2];

static struct centroid prev_centroids[MAX_NUM_BOUNDING_BOXES];
static uint8_t prev_num_bboxes = 0;
static struct centroid centroids[MAX_NUM_BOUNDING_BOXES];
static uint8_t num_bboxes = 0;
static int total_crossings = 0;
static int left_crossings = 0;
static int right_crossings = 0;
void people_counting_init(void)
{
  // Setup the camera and all the necessary buffers
  printf("Init...\n");
  mlx90640_init(sl_i2cspm_sensor);
  mlx90640_SetRefreshRate(0x06);

  raw_img.type = IMAGEFORMAT_FLOAT;
  raw_img.width = MLX90640_WIDTH;
  raw_img.height = MLX90640_HEIGHT;
  raw_img.depth = 1;
  raw_img.data.f = ir_img_buf;

  model_input = sl_tflite_micro_get_input_tensor();
  model_input_img.type = IMAGEFORMAT_FLOAT;
  model_input_img.width = MLX90640_WIDTH;
  model_input_img.height = MLX90640_HEIGHT;
  model_input_img.depth = 1;
  model_input_img.data.f = model_input->data.f;

  interpreter = sl_tflite_micro_get_interpreter();
  heatmap_img.type = IMAGEFORMAT_FLOAT;
  heatmap_img.width = OUTPUT_WIDTH;
  heatmap_img.height = OUTPUT_HEIGHT;
  heatmap_img.depth = OUTPUT_DEPTH;
  heatmap_img.data.f = interpreter->output(0)->data.f;

  label_image.data.f = label_image_buf;
  label_image.type = IMAGEFORMAT_UINT8;
  label_image.width = OUTPUT_WIDTH;
  label_image.height = OUTPUT_HEIGHT;

  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1)
      || (model_input->dims->data[1] != MLX90640_HEIGHT)
      || (model_input->dims->data[2] != MLX90640_WIDTH)
      || (model_input->type != kTfLiteFloat32)) {
    app_log_error("Bad input tensor parameters in model.\n");
    EFM_ASSERT(false);
    return;
  }
}
float process_func_normalize(float val)
{
  val = val / 60;
  return val;
}
void people_counting_process(void)
{
  // Obtain and preprocess the image
  mlx90640_get_image_array(raw_img.data.f);
  process_image(&model_input_img, &raw_img, process_func_normalize);

  // Perform inference
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    app_log_error("Inference failed!\n");
    return;
  }

  // Postprocess into a final set of bounding boxes
  num_bboxes = output_to_bboxes(&heatmap_img, bboxes);
  num_bboxes = non_max_suppression(bboxes, num_bboxes, final_bboxes, IOU_THRESHOLD);

  // Find and track centroids
  find_centroids_bboxes(final_bboxes, num_bboxes, centroids);
  update_centroid_connections(prev_centroids, prev_num_bboxes, centroids, num_bboxes, MAX_TRACKING_DIST);

  // Count crossings using centroids
  int crossing = count_centroid_crossings(centroids, num_bboxes, 20);
  if (crossing > 0) {
    right_crossings++;
  } else if (crossing < 0) {
    left_crossings++;
  }
  total_crossings += crossing;

  // Output results

  char str[100];
  sprintf(str, "Crossed (L/R/T): %d/%d/%d, Present: %d", left_crossings, right_crossings, total_crossings, num_bboxes);
  #if OUTPUT_OVER_BLE
  export_image_bt(&raw_img, "image", str);
  export_bboxes_over_serial_bt(final_bboxes, num_bboxes, 2);
  export_centroids_over_serial_bt(centroids, num_bboxes, 2);
  #else
  export_image(&raw_img, "image", str, sl_iostream_vcom_handle);
  export_bboxes_over_serial(final_bboxes, num_bboxes, 2);
  export_centroids_over_serial(centroids, num_bboxes, 2);
  #endif

// Save information for the next frame
  memcpy(prev_centroids, centroids, sizeof(centroid) * num_bboxes);
  prev_num_bboxes = num_bboxes;
}
/**
 * @brief Converts the output-data into bounding boxes.
 *
 * @param img The data
 * @param bboxes The array in which to place the bounding boxes
 * @return uint8_t Number of bounding boxes that were placed into the array
 */
uint8_t output_to_bboxes(const struct Image* img, struct bbox bboxes[])
{
  float confidence_threshold_squared = CONFIDENCE_THRESHOLD * CONFIDENCE_THRESHOLD;
  uint8_t counter = 0;
  for (size_t y = 0; y < img->height; y++) {
    for (size_t x = 0; x < img->width; x++) {
      // Extract the data for this pixel
      size_t idx = get_index(x, y, 0, img->width, img->height, img->depth);
      float l = img->data.f[idx + 0];
      float t = img->data.f[idx + 1];
      float r = img->data.f[idx + 2];
      float b = img->data.f[idx + 3];
      float c = img->data.f[idx + 4];
      float centerness = img->data.f[idx + 5];
      // Filter out pixels that have low confidence
      float confidence_squared = c * centerness;
      if (confidence_squared < confidence_threshold_squared) {
        continue;
      }
      // Convert l,t,r,b coordinates to x0,x1,y0,y1 coordinates
      // Also scale the bounding box to the same scale as the input
      float x0 = (x - l) * INPUT_OUTPUT_RATIO;
      float x1 = (r + x) * INPUT_OUTPUT_RATIO;
      float y0 = (y - t) * INPUT_OUTPUT_RATIO;
      float y1 = (b + y) * INPUT_OUTPUT_RATIO;

      x0 = clampf(x0, 0, MLX90640_WIDTH);
      x1 = clampf(x1, 0, MLX90640_WIDTH);
      y0 = clampf(y0, 0, MLX90640_HEIGHT);
      y1 = clampf(y1, 0, MLX90640_HEIGHT);
      // Calculate the confidence score
      float confidence = 0;
      arm_sqrt_f32(confidence_squared, &confidence);
      //printf ("%f,%f,%f,%f, %f\n", x0, y0, x1, y1, confidence);
      bboxes[counter] = { .x = x0, .y = y0, .width = x1 - x0, .height = y1 - y0, .class_id = 1, .confidence = confidence };
      counter++;
    }
  }
  return counter;
}
/**
 * @brief Count how many people have crossed an imaginary line at position x
 *
 * @param centroids Array of centroids with updated connections
 * @param num_labels Number of centroids
 * @param x position of the line
 * @return int How many have crossed, the sign is the direction
 */
int count_centroid_crossings(struct centroid centroids[], uint8_t num_labels, uint32_t x)
{
  int num_crossed = 0;
  for (int label_id = 0; label_id < num_labels; label_id++) {
    struct centroid* c = &centroids[label_id];
    if (c->prev_centroid == NULL) {
      continue;
    }
    if (c->x > x && c->prev_centroid->x < x) {
      num_crossed++;
    } else if (c->x < x && c->prev_centroid->x > x) {
      num_crossed--;
    }
  }
  return num_crossed;
}
void _printf_bt(const char* format, ...)
{
  char buffer[256];
  va_list args;
  va_start(args, format);
  int len = vsnprintf(buffer, 255, format, args);

  data_notify(buffer, len);

  va_end(args);
}
void export_image_bt(const struct Image *img, const char *title, const char *misc_info)
{
  _printf_bt("image:%s,%d,%d,%d,%d,%s\n", title, img->width, img->height, img->depth, (int)img->type, misc_info);
  size_t len = img->width * img->height * img->depth * sizeof_imageformat(img->type);
  data_notify(img->data.raw, len);
  _printf_bt("\n");
}
void export_bboxes_over_serial_bt(const struct bbox bboxes[], uint8_t num_boxes, uint8_t precision)
{
  _printf_bt("bboxes:%i\n", num_boxes);
  for (uint8_t i = 0; i < num_boxes; i++) {
    _printf_bt("%.*f,%.*f,%.*f,%.*f,%.*f,%d\n",
               precision, bboxes[i].x,
               precision, bboxes[i].y,
               precision, bboxes[i].width,
               precision, bboxes[i].height,
               precision, bboxes[i].confidence,
               bboxes[i].class_id
               );
  }
}
void export_centroids_over_serial_bt(const struct centroid centroids[], uint8_t num_labels, uint8_t precision)
{
  _printf_bt("centroids:%i\n", num_labels);
  for (uint8_t i = 0; i < num_labels; i++) {
    if (centroids[i].prev_centroid == NULL) {
      _printf_bt("%.*f,%.*f,%d\n",
                 precision, centroids[i].x,
                 precision, centroids[i].y,
                 centroids[i].count
                 );
    } else {
      _printf_bt("%.*f,%.*f,%d-%.*f,%.*f,%d,%.*f\n",
                 precision, centroids[i].x,
                 precision, centroids[i].y,
                 centroids[i].count,
                 precision, centroids[i].prev_centroid->x,
                 precision, centroids[i].prev_centroid->y,
                 centroids[i].prev_centroid->count,
                 precision, centroids[i].prev_centroid_dist_squared
                 );
    }
  }
}

#include "image_utils.h"
#include "math.h"
#include "sl_udelay.h"
#include <algorithm>    // std::sort
#include <vector>
uint8_t sizeof_imageformat(enum ImageFormat format)
{
  switch (format) {
    case IMAGEFORMAT_FLOAT:
      return sizeof(float);
    case IMAGEFORMAT_UINT8:
      return sizeof(uint8_t);
  }
  return 0;
}
/**
 * @brief Fast clamp function that can compile into only 3(!) instructions. https://stackoverflow.com/a/16659263
 *
 * @param d
 * @param min
 * @param max
 * @return float
 */
float clampf(float d, float min, float max)
{
  const float t = d < min ? min : d;
  return t > max ? max : t;
}
/**
 * @brief Convert x,y,z to a 1D index used for accessing pixel values
 *
 * @param x
 * @param y
 * @param z
 * @param width
 * @param depth
 * @return size_t
 */
size_t get_index(size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth)
{
  return z + x * depth + width * y * depth;
}
template <typename T>
T _get_pixel_value(const struct Image *img, size_t x, size_t y, size_t z)
{
  return ((T *)img->data.raw)[get_index(x, y, z, img->width, img->height, img->depth)];
}
template <typename T>
void _set_pixel_value(const struct Image *img, size_t x, size_t y, size_t z, T val)
{
  ((T *)img->data.raw)[get_index(x, y, z, img->width, img->height, img->depth)] = val;
}
void center_crop(uint8_t *dst_img, size_t new_width, size_t new_height, uint8_t *src_img, size_t width, size_t height)
{
  size_t offset_x = (width - new_width) / 2;
  size_t offset_y = (height - new_height) / 2;
  size_t i = 0;
  for (size_t y = offset_y; y < height - offset_y; y++) {
    for (size_t x = offset_x; x < width - offset_x; x++) {
      uint8_t val = src_img[x + width * y];
      dst_img[i] = val;
      i++;
    }
  }
}
/**
 * Does not use floating point, so it should be the fastest way
 * Source: https://tech-algorithm.com/articles/nearest-neighbor-image-scaling
 */
void scale_nearest_neighbour(uint8_t *dst_img, size_t new_width, size_t new_height, uint8_t *src_img, size_t width, size_t height)
{
  size_t ratio_x = (size_t)((width << 16) / new_width) + 1;
  size_t ratio_y = (size_t)((height << 16) / new_height) + 1;
  size_t x2, y2;
  for (size_t y = 0; y < new_height; y++) {
    y2 = ((y * ratio_y) >> 16);
    for (size_t x = 0; x < new_width; x++) {
      x2 = ((x * ratio_x) >> 16);
      dst_img[(y * new_width) + x] = src_img[(y2 * width) + x2];
    }
  }
}
/**
 * Source: https://tech-algorithm.com/articles/bilinear-image-scaling/
 */
void scale_bilinear(uint8_t *dst_img, size_t new_width, size_t new_height, uint8_t *src_img, size_t width, size_t height)
{
  float ratio_x = ((float)(width - 1)) / new_width;
  float ratio_y = ((float)(height - 1)) / new_height;
  size_t i = 0;
  for (size_t y = 0; y < new_height; y++) {
    float y2 = (size_t)(y * ratio_y);
    float y_diff = (y * ratio_y) - y2;
    for (size_t x = 0; x < new_width; x++) {
      float x2 = (size_t)(x * ratio_x);
      float x_diff = (x * ratio_x) - x2;
      size_t index = y2 * width + x2;

      uint8_t A = src_img[index] & 0xFF;
      uint8_t B = src_img[index + 1] & 0xFF;
      uint8_t C = src_img[index + width] & 0xFF;
      uint8_t D = src_img[index + width + 1] & 0xFF;
      uint8_t g = (uint8_t)(
        A * (1 - x_diff) * (1 - y_diff)
        + B * (x_diff) * (1 - y_diff)
        + C * (y_diff) * (1 - x_diff)
        + D * (x_diff * y_diff)
        );
      dst_img[i] = g;
      i++;
    }
  }
}
template <typename T>
void _process_image(const struct Image *dst_img, const struct Image *src_img, process_func_t process_func)
{
  for (size_t y = 0; y < src_img->height; y++) {
    for (size_t x = 0; x < src_img->width; x++) {
      T val = _get_pixel_value<T>(src_img, x, y, 0);
      val = process_func(val);
      _set_pixel_value<T>(dst_img, x, y, 0, val);
    }
  }
}
void process_image(const struct Image *dst_img, const struct Image *src_img, process_func_t process_func)
{
  switch (src_img->type) {
    case IMAGEFORMAT_FLOAT:
      return _process_image<float>(dst_img, src_img, process_func);
    case IMAGEFORMAT_UINT8:
      return _process_image<u_int8_t>(dst_img, src_img, process_func);
  }
}

template <typename T>
uint8_t _find_connected_pixels(const struct Image *dst_label_img, const struct Image *src_img, float threshold, struct queue_pixel_entry* working_memory, uint32_t working_memory_len)
{
  static int dxs[] = { 0, 1, -1 };
  static int dys[] = { 0, 1, -1 };
  memset(working_memory, 0, working_memory_len * sizeof(queue_pixel_entry));
  uint32_t queue_ptr = 0;
  sl_slist_node_t *head;
  sl_slist_init(&head);

  uint8_t current_label = 0;
  for (size_t y = 0; y < src_img->height; y++) {
    for (size_t x = 0; x < src_img->width; x++) {
      if (_get_pixel_value<T>(src_img, x, y, 0) < threshold) {
        continue;
      }
      if (_get_pixel_value<uint8_t>(dst_label_img, x, y, 0) > 0) {
        continue;
      }
      current_label++;
      _set_pixel_value<uint8_t>(dst_label_img, x, y, 0, current_label);
      working_memory[queue_ptr] = { .x = (int)x, .y = (int)y };
      sl_slist_push_back(&head, &working_memory[queue_ptr].node);
      queue_ptr++;
      while (head != NULL) {
        sl_slist_node* current_node = sl_slist_pop(&head);

        struct queue_pixel_entry *current = SL_SLIST_ENTRY(current_node, struct queue_pixel_entry, node);
        for (int i_dy = 0; i_dy < (int)(sizeof(dys) / sizeof(dys[0])); i_dy++) {
          for (int i_dx = 0; i_dx < (int)(sizeof(dxs) / sizeof(dxs[0])); i_dx++) {
            int dx = dxs[i_dx];
            int dy = dys[i_dy];
            if (dx == 0 && dy == 0) {
              continue;
            }
            int next_x = current->x + dx;
            int next_y = current->y + dy;
            if (next_x < 0 || next_x >= (int)src_img->width) {
              continue;
            }
            if (next_y < 0 || next_y >= (int)src_img->height) {
              continue;
            }
            if (_get_pixel_value<T>(src_img, next_x, next_y, 0) >= threshold && _get_pixel_value<uint8_t>(dst_label_img, next_x, next_y, 0) == 0) {
              _set_pixel_value<uint8_t>(dst_label_img, next_x, next_y, 0, current_label);
              working_memory[queue_ptr] =  {   .x = next_x, .y = next_y };
              sl_slist_push_back(&head, &working_memory[queue_ptr].node);
              queue_ptr++;
            }
          }
        }
      }
    }
  }
  return current_label;
}
/**
 * @brief Finds connected pixels and outputs a label image containing that information.
 *
 * @param dst_label_img The destination label image
 * @param src_img  The source image to run the algorithm on
 * @param threshold The threshold to turn floats into 1s and 0s
 * @param working_memory The working memory for the algorithm
 * @param working_memory_len The size of the working memory for the algorithm
 * @return uint8_t Number of unique blobs of connected pixels
 */
uint8_t find_connected_pixels(const struct Image *dst_label_img, const struct Image *src_img, float threshold, struct queue_pixel_entry* working_memory, uint32_t working_memory_len)
{
  switch (src_img->type) {
    case IMAGEFORMAT_FLOAT:
      return _find_connected_pixels<float>(dst_label_img, src_img, threshold, working_memory, working_memory_len);
    case IMAGEFORMAT_UINT8:
      return _find_connected_pixels<uint8_t>(dst_label_img, src_img, threshold, working_memory, working_memory_len);
  }
  return 0;
}
float calculate_iou(const struct bbox* a, const struct bbox* b)
{
  float xA = std::max<float>(a->x, b->x);
  float yA = std::max<float>(a->y, b->y);
  float xB = std::min<float>(a->x + a->width, b->x + b->width);
  float yB = std::min<float>(a->y + a->height, b->y + b->height);

  float intersection_area = std::abs(std::max<float>(xB - xA, 0) * std::max<float>(yB - yA, 0));
  if (intersection_area == 0) {
    return 0;
  }
  float area_a = a->width * a->height;
  float area_b = b->width * b->height;

  float iou = intersection_area / (area_a + area_b - intersection_area);
  return iou;
}
/**
 * @brief Sort function for bounding boxes, results in descending order.
 *
 * @param bboxes
 * @param max_num_bboxes
 * @param confidence_threshold
 */
bool bbox_sort(const struct bbox* a, const struct bbox *b)
{
  return (a->confidence < b->confidence);
}
/**
 * @brief Non-max suppression algorithm
 *
 * @param bboxes The raw bounding boxes
 * @param max_num_bboxes The maximum number of raw bounding boxes, also the size of the array
 * @param bboxes_out The output array to place the remaining bounding boxes after the algorithm has done its job
 * @param confidence_threshold The confidence threshold for which to even consider bounding boxes in the first place
 * @param iou_threshold Bounding boxes that have a greater overlap than this threshold will be competing against eachother
 * @return uint8_t Number of remaining bounding boxes
 */
uint8_t non_max_suppression(struct bbox bboxes[], uint8_t max_num_bboxes, struct bbox bboxes_out[], float iou_threshold)
{
  static std::vector<struct bbox*> to_check;
  for (uint8_t i = 0; i < max_num_bboxes; i++) {
    struct bbox* bb = &bboxes[i];
    to_check.push_back(bb);
  }
  // Sort so that the best bbox is always in the back
  std::sort(to_check.begin(), to_check.end(), bbox_sort);
  uint8_t num_kept_bboxes = 0;
  while (!to_check.empty()) {
    struct bbox* best_bb = to_check.back();
    //Save best in output
    bboxes_out[num_kept_bboxes] = *best_bb;
    num_kept_bboxes++;
    to_check.pop_back();
    // Iterate backwards so that we can safely remove elements, need to use int here
    for (int i = to_check.size() - 1; i >= 0; i--) {
      const struct bbox* bb = to_check.at(i);
      float iou = calculate_iou(best_bb, bb);
      if (iou > iou_threshold) {
        to_check.erase(to_check.begin() + i);
      }
    }
  }
  return num_kept_bboxes;
}
template <typename T>
void _find_centroids_connected_pixels(const struct Image *label_img, const struct Image *src_img, const struct Image *target_scale_img, struct centroid centroids_out[], uint8_t num_labels)
{
  float ratio_x = (float)target_scale_img->width / (float)label_img->width;
  float ratio_y = (float)target_scale_img->height / (float)label_img->height;
  uint16_t counts[num_labels] = { 0 };
  float x_vals[num_labels] = { 0 };
  float y_vals[num_labels] = { 0 };
  for (size_t y = 0; y < label_img->height; y++) {
    for (size_t x = 0; x < label_img->width; x++) {
      uint8_t label = _get_pixel_value<uint8_t>(label_img, x, y, 0);
      if (label == 0) {
        continue;
      }
      counts[label - 1]++;
      T val = _get_pixel_value<T>(src_img, x, y, 0);
      // Sum the coordinates of a label weighted by confidence
      x_vals[label - 1] += ((float)x + 0.5f) * val;
      y_vals[label - 1] += ((float)y + 0.5f) * val;
    }
  }
  for (uint8_t label_id = 0; label_id < num_labels; label_id++) {
    // Take the average of the coordinate sums, and resize the coordinates to the same resolution as the raw input image.
    centroids_out[label_id].x = (x_vals[label_id] / counts[label_id]) * ratio_x;
    centroids_out[label_id].y = (y_vals[label_id] / counts[label_id]) * ratio_y;
    centroids_out[label_id].count = counts[label_id];
  }
}
/**
 * @brief Finds centroids from connected pixels represented in a label image
 *
 * @param label_img The label image of the connected pixels
 * @param src_img The source image, same as the one used to calculate the label image.
 * @param raw_img The raw camera image, used to calculate scaling
 * @param centroids_out The centroids output array
 * @param num_labels The number of unique blobs on connected pixels
 */
void find_centroids_connected_pixels(const struct Image *label_img, const struct Image *src_img, const struct Image *raw_img, struct centroid centroids_out[], uint8_t num_labels)
{
  switch (src_img->type) {
    case IMAGEFORMAT_FLOAT:
      return _find_centroids_connected_pixels<float>(label_img, src_img, raw_img, centroids_out, num_labels);
    case IMAGEFORMAT_UINT8:
      return _find_centroids_connected_pixels<uint8_t>(label_img, src_img, raw_img, centroids_out, num_labels);
  }
}
/**
 * @brief Given a list of bounding boxes, output a list of centroids
 *
 * @param bboxes
 * @param num_bboxes
 * @param centroids_out
 */
void find_centroids_bboxes(struct bbox bboxes[], uint8_t num_bboxes, struct centroid centroids_out[])
{
  for (uint8_t i = 0; i < num_bboxes; i++) {
    float cx = bboxes[i].x + bboxes[i].width / 2;
    float cy = bboxes[i].y + bboxes[i].height / 2;
    uint16_t count = (int)(bboxes[i].width * bboxes[i].height);
    centroids_out[i] = { .x = cx, .y = cy, .count = count };
  }
}
/**
 * @brief Function that keeps track of centroids over time.
 * Based on the assumption that the object closest in the current timestep to an object in the previous timestep, is the same object.
 *
 * @param centroids_prev The centroids in the previous timestep
 * @param num_labels_prev The number of centroids in the previous timestep
 * @param centroids_now The centroids in the current timestep
 * @param num_labels_now The number of centroids in the current timestep
 * @param max_dist Maximum distance to consider objects as the same
 */
void update_centroid_connections(struct centroid centroids_prev[], uint8_t num_labels_prev, struct centroid centroids_now[], uint8_t num_labels_now, float max_dist)
{
  float squared_max_dist = max_dist * max_dist;
  for (uint8_t label_id_now = 0; label_id_now < num_labels_now; label_id_now++) {
    struct centroid *centroid_now = &centroids_now[label_id_now];
    centroid_now->prev_centroid = NULL;
    centroid_now->prev_centroid_dist_squared = -1;
    for (uint8_t label_id_prev = 0; label_id_prev < num_labels_prev; label_id_prev++) {
      struct centroid *centroid_prev = &centroids_prev[label_id_prev];
      float delta_x = centroid_now->x - centroid_prev->x;
      float delta_y = centroid_now->y - centroid_prev->y;
      float squared_dist = (delta_x * delta_x) + (delta_y * delta_y);
      if (max_dist != -1 && squared_dist > squared_max_dist) {
        continue;
      }
      if (centroid_now->prev_centroid_dist_squared == -1 || squared_dist < centroid_now->prev_centroid_dist_squared) {
        // If the previous centroid has a next centroid
        if (centroid_prev->next_centroid != NULL) {
          // If it has a better distance measure than ours, skip
          if (centroid_prev->next_centroid_dist_squared < squared_dist) {
            continue;
          }
          // If this centroid is closer, we disconnect any other current centroid to that is currently connected to the previous centroid
          centroid_prev->next_centroid->prev_centroid = NULL;
          centroid_prev->next_centroid->prev_centroid_dist_squared = -1;
        }

        // We add a new connection to the current centroid we are checking
        centroid_now->prev_centroid = centroid_prev;
        centroid_now->prev_centroid_dist_squared = squared_dist;

        centroid_prev->next_centroid = centroid_now;
        centroid_prev->next_centroid_dist_squared = squared_dist;
      }
    }
  }
}
void export_image(const struct Image *img, const char *title, const char *misc_info, sl_iostream_t *handle)
{
  printf("image:%s,%d,%d,%d,%d,%s\n", title, img->width, img->height, img->depth, (int)img->type, misc_info);
  sl_iostream_write(handle, img->data.raw, img->width * img->height * img->depth * sizeof_imageformat(img->type));
  printf("\n");
}
void export_centroids_over_serial(const struct centroid centroids[], uint8_t num_labels, uint8_t precision)
{
  printf("centroids:%i\n", num_labels);
  for (uint8_t i = 0; i < num_labels; i++) {
    if (centroids[i].prev_centroid == NULL) {
      printf("%.*f,%.*f,%d\n",
             precision, centroids[i].x,
             precision, centroids[i].y,
             centroids[i].count
             );
    } else {
      printf("%.*f,%.*f,%d-%.*f,%.*f,%d,%.*f\n",
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
void export_bboxes_over_serial(const struct bbox bboxes[], uint8_t num_boxes, uint8_t precision)
{
  printf("bboxes:%i\n", num_boxes);
  for (uint8_t i = 0; i < num_boxes; i++) {
    printf("%.*f,%.*f,%.*f,%.*f,%.*f,%d\n",
           precision, bboxes[i].x,
           precision, bboxes[i].y,
           precision, bboxes[i].width,
           precision, bboxes[i].height,
           precision, bboxes[i].confidence,
           bboxes[i].class_id
           );
  }
}
void print_image(const struct Image *img)
{
  for (size_t y = 0; y < img->height; y++) {
    for (size_t x = 0; x < img->width; x++) {
      switch (img->type) {
        case IMAGEFORMAT_UINT8:
          printf("%i,", _get_pixel_value<uint8_t>(img, x, y, 0));
          break;
        case IMAGEFORMAT_FLOAT:
          printf("%f,", _get_pixel_value<float>(img, x, y, 0));
          break;
      }
    }
    printf("\n");
  }
}

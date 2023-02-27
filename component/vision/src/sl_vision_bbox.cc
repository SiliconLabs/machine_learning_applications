#include <algorithm>    // std::sort
#include <vector>
#include "sl_vision_image.hpp"
#include "sl_vision_bbox.h"
#include "sl_vision_bbox.hpp"

float sl_vision_bbox_calculate_iou(const sl_vision_bbox_t* a, const sl_vision_bbox_t* b)
{
  float xA = std::max<float>(a->x, b->x);
  float yA = std::max<float>(a->y, b->y);
  float xB = std::min<float>(a->x + a->width, b->x + b->width);
  float yB = std::min<float>(a->y + a->height, b->y + b->height);

  float xDist = xB - xA;
  float yDist = yB - yA;
  float intersection_area = 0;
  if (xDist > 0 && yDist > 0) {
    intersection_area = xDist * yDist;
  } else {
    return 0;
  }
  float area_a = a->width * a->height;
  float area_b = b->width * b->height;

  float iou = intersection_area / (area_a + area_b - intersection_area);
  return iou;
}

size_t sl_vision_bbox_non_max_suppression(sl_vision_bbox_t bboxes[], size_t max_num_bboxes, sl_vision_bbox_t bboxes_out[], float iou_threshold)
{
  auto sort_function = [](const sl_vision_bbox_t* a, const sl_vision_bbox_t *b) {
                         return (a->confidence < b->confidence);
                       };
  static std::vector<sl_vision_bbox_t*> to_check;
  for (size_t i = 0; i < max_num_bboxes; i++) {
    sl_vision_bbox_t* bb = &bboxes[i];
    to_check.push_back(bb);
  }
  // Sort so that the best bbox is always in the back
  std::sort(to_check.begin(), to_check.end(), sort_function);
  size_t num_kept_bboxes = 0;
  while (!to_check.empty()) {
    sl_vision_bbox_t* best_bb = to_check.back();
    //Save best in output
    bboxes_out[num_kept_bboxes] = *best_bb;
    num_kept_bboxes++;
    to_check.pop_back();
    // Iterate backwards so that we can safely remove elements, need to use int here (or do we?) 
    for (int i = to_check.size() - 1; i >= 0; i--) {
      const sl_vision_bbox_t* bb = to_check.at(i);
      float iou = sl_vision_bbox_calculate_iou(best_bb, bb);
      if (iou > iou_threshold) {
        to_check.erase(to_check.begin() + i);
      }
    }
  }
  return num_kept_bboxes;
}

void sl_vision_bbox_blur(const sl_vision_image_t* img, const sl_vision_bbox_t* bb, size_t kernel_size)
{
  switch (img->format) {
    case IMAGEFORMAT_UINT8:
      generic_sl_vision_bbox_blur<uint8_t>(img, bb, kernel_size);
      break;
    case IMAGEFORMAT_FLOAT:
      generic_sl_vision_bbox_blur<float>(img, bb, kernel_size);
      break;
    default:
      printf("Unsupported image format in file %s:%d!\n", __FILE__, __LINE__);
      exit(1);
  }
}

void sl_vision_bbox_pixelize(const sl_vision_image_t* img, const sl_vision_bbox_t* bb, size_t pixel_size)
{
  switch (img->format) {
    case IMAGEFORMAT_UINT8:
      generic_sl_vision_bbox_pixelize<uint8_t>(img, bb, pixel_size);
      break;
    case IMAGEFORMAT_FLOAT:
      generic_sl_vision_bbox_pixelize<float>(img, bb, pixel_size);
      break;
    default:
      printf("Unsupported image format in file %s:%d!\n", __FILE__, __LINE__);
      exit(1);
  }
}

void sl_vision_bbox_export_over_serial(const sl_vision_bbox_t bboxes[], uint8_t num_boxes, uint8_t precision)
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

#ifndef SL_VISION_BBOX_H
#define SL_VISION_BBOX_H

#include <stdio.h>
#include "sl_slist.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct  {
  float x;
  float y;
  float width;
  float height;
  uint16_t class_id;
  float confidence;
} sl_vision_bbox_t;
struct queue_bbox_entry{
  sl_slist_node_t node;
  const sl_vision_bbox_t* bb;
};
/**
 * @brief Calculate the intersection over union of two bounding boxes
 *
 * @param a
 * @param b
 * @return Intersection over union as a float
 */
float sl_vision_bbox_calculate_iou(const sl_vision_bbox_t* a, const sl_vision_bbox_t* b);
/**
 * @brief Non-max suppression algorithm
 *
 * @param bboxes The raw bounding boxes
 * @param max_num_bboxes The maximum number of raw bounding boxes, also the size of the input bbox array
 * @param bboxes_out The output array to place the remaining bounding boxes after the algorithm has done its job
 * @param iou_threshold Bounding boxes that have a greater overlap than this threshold will be competing against eachother
 * @return uint16_t Number of remaining bounding boxes
 */
size_t sl_vision_bbox_non_max_suppression(sl_vision_bbox_t bboxes[], size_t max_num_bboxes, sl_vision_bbox_t bboxes_out[], float iou_threshold);
/**
 * @brief Blur a bounding box
 *
 * @param img The input image
 * @param bb The bounding box to blur
 */
void sl_vision_bbox_blur(const sl_vision_image_t* img, const sl_vision_bbox_t* bb, size_t kernel_size);
/**
 * @brief Pixelize a bounding box
 *
 * @param img The input image
 * @param bb The bounding box to pixelize
 * @param pixel_size The pixel size
 */
void sl_vision_bbox_pixelize(const sl_vision_image_t* img, const sl_vision_bbox_t* bb, size_t pixel_size);
/**
 * @brief Export bounding boxes over serial
 *
 * @param bboxes
 * @param num_boxes
 * @param precision
 */
void sl_vision_bbox_export_over_serial(const sl_vision_bbox_t bboxes[], uint8_t num_boxes, uint8_t precision);
#ifdef __cplusplus
}
#endif

#endif // SL_VISION_BBOX_H

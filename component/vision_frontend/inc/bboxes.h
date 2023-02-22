#ifndef BBOXES_H
#define BBOXES_H

#include <stdio.h>
#include "sl_slist.h"

#ifdef __cplusplus
extern "C" {
#endif
struct queue_bbox_entry{
  sl_slist_node_t node;
  const struct bbox* bb;
};
struct bbox {
  float x;
  float y;
  float width;
  float height;
  uint16_t class_id;
  float confidence;
};
/**
 * @brief Calculate the intersection over union of two bounding boxes
 *
 * @param a
 * @param b
 * @return Intersection over union as a float
 */
float calculate_iou(const struct bbox* a, const struct bbox* b);
/**
 * @brief Non-max suppression algorithm
 *
 * @param bboxes The raw bounding boxes
 * @param max_num_bboxes The maximum number of raw bounding boxes, also the size of the input bbox array
 * @param bboxes_out The output array to place the remaining bounding boxes after the algorithm has done its job
 * @param iou_threshold Bounding boxes that have a greater overlap than this threshold will be competing against eachother
 * @return uint16_t Number of remaining bounding boxes
 */
size_t non_max_suppression(struct bbox bboxes[], size_t max_num_bboxes, struct bbox bboxes_out[], float iou_threshold);
/**
 * @brief Blur a bounding box
 *
 * @param img The input image
 * @param bb The bounding box to blur
 */
void blur_bbox(const struct Image* img, const struct bbox* bb, size_t kernel_size);
/**
 * @brief Pixelize a bounding box
 *
 * @param img The input image
 * @param bb The bounding box to pixelize
 * @param pixel_size The pixel size
 */
void pixelize_bbox(const struct Image* img, const struct bbox* bb, size_t pixel_size);
/**
 * @brief Export bounding boxes over serial
 *
 * @param bboxes
 * @param num_boxes
 * @param precision
 */
void export_bboxes_over_serial(const struct bbox bboxes[], uint8_t num_boxes, uint8_t precision);
#ifdef __cplusplus
}
#endif

#endif /* BBOXES_H */

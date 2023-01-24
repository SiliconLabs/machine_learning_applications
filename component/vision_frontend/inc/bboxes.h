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
float calculate_iou(const struct bbox* a, const struct bbox* b);
size_t non_max_suppression(struct bbox bboxes[], size_t max_num_bboxes, struct bbox bboxes_out[], float iou_threshold);
void blur_bbox(const struct Image* img, const struct bbox* bb, size_t kernel_size);
void pixelize_bbox(const struct Image* img, const struct bbox* bb, size_t pixel_size);
void export_bboxes_over_serial(const struct bbox bboxes[], uint8_t num_boxes, uint8_t precision);
#ifdef __cplusplus
}
#endif

#endif /* BBOXES_H */

#ifndef PEOPLE_COUNTING_H
#define PEOPLE_COUNTING_H

#include "sl_vision.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif
void people_counting_init(void);
void people_counting_process(void);
uint8_t output_to_bboxes(const sl_vision_image_t* img, sl_vision_bbox_t bboxes[]);
int count_centroid_crossings(sl_vision_centroid_t centroids[], uint8_t num_labels, uint32_t x);
void export_image_bt(const  sl_vision_image_t *img, const char *title, const char *misc_info);
void export_bboxes_over_serial_bt(const  sl_vision_bbox_t bboxes[], uint8_t num_boxes, uint8_t precision);
void export_centroids_over_serial_bt(const  sl_vision_centroid_t centroids[], uint8_t num_labels, uint8_t precision);
#ifdef __cplusplus
}
#endif

#endif // PEOPLE_COUNTING_H

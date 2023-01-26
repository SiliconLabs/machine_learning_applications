#ifndef PEOPLE_COUNTING_H
#define PEOPLE_COUNTING_H

#include "image_utils/image_utils.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif
void people_counting_init(void);
void people_counting_process(void);
uint8_t output_to_bboxes(const struct Image* img, struct bbox bboxes[]);
int count_centroid_crossings(struct centroid centroids[], uint8_t num_labels, uint32_t x);
void export_image_bt(const struct Image *img, const char *title, const char *misc_info);
void export_bboxes_over_serial_bt(const struct bbox bboxes[], uint8_t num_boxes, uint8_t precision);
void export_centroids_over_serial_bt(const struct centroid centroids[], uint8_t num_labels, uint8_t precision);
#ifdef __cplusplus
}
#endif

#endif // PEOPLE_COUNTING_H

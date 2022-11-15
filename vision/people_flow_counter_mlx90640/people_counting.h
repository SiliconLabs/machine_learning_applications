#ifndef PEOPLE_COUNTING_H
#define PEOPLE_COUNTING_H

#include "image_utils/image_utils.h"
#ifdef __cplusplus
extern "C" {
#endif
void people_counting_init(void);
void people_counting_process(void);
uint8_t output_to_bboxes(const struct Image* img, struct bbox bboxes[]);
int count_centroid_crossings(struct centroid centroids[], uint8_t num_labels, uint32_t x);
#ifdef __cplusplus
}
#endif

#endif // PEOPLE_COUNTING_H

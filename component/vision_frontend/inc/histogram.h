#ifndef HISTOGRAM_H
#define HISTOGRAM_H
#include "image_utils.h"
#ifdef __cplusplus
extern "C" {
#endif
void calculate_cdf(const size_t* hist, size_t* cdf, size_t hist_levels);
void histogram_equalize(const struct Image* src_img, const struct Image* dst_img);
#ifdef __cplusplus
}
#endif
#endif /* HISTOGRAM_H */

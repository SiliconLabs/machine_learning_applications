#ifndef HISTOGRAM_H
#define HISTOGRAM_H
#include "image_utils.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Calculate the cumulative distribution function of a histogram
 *
 * @param hist The histogram
 * @param cdf The output cdf
 * @param hist_levels The number of histogram levels
 */
void calculate_cdf(const size_t* hist, size_t* cdf, size_t hist_levels);

/**
 * @brief Equalize the histogram of an image
 *
 * @param src_img
 * @param dst_img
 */
void histogram_equalize(const struct Image* src_img, const struct Image* dst_img);
#ifdef __cplusplus
}
#endif
#endif /* HISTOGRAM_H */

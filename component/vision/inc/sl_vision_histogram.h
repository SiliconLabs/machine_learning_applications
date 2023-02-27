#ifndef SL_VISION_HISTOGRAM_H
#define SL_VISION_HISTOGRAM_H
#include "sl_vision_image.h"
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
void sl_vision_histogram_cdf(const size_t* hist, size_t* cdf, size_t hist_levels);

/**
 * @brief Equalize the histogram of an image
 *
 * @param src_img
 * @param dst_img
 */
void sl_vision_histogram_equalize(const sl_vision_image_t* src_img, const sl_vision_image_t* dst_img);
#ifdef __cplusplus
}
#endif
#endif // SL_VISION_HISTOGRAM_H

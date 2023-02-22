#include "histogram.h"
#include "image_utils.hpp"

#define HIST_LEVELS 256

/**
 * @brief Calculate the cumulative distribution function of a histogram
 *
 * @param hist The histogram
 * @param cdf The output cdf
 * @param hist_levels The number of histogram levels
 */
void calculate_cdf(const size_t* hist, size_t* cdf, size_t hist_levels)
{
  cdf[0] = hist[0];
  for (size_t i = 1; i < hist_levels; i++) {
    cdf[i] = cdf[i - 1] + hist[i];
  }
}

/**
 * @brief Equalize the histogram of an image
 *
 * @param src_img
 * @param dst_img
 */
void histogram_equalize(const struct Image* src_img, const struct Image* dst_img)
{
  if (src_img->format != IMAGEFORMAT_UINT8) {
    printf("Only uint8 is supported!\n");
    return;
  }
  if (src_img->depth != 1) {
    printf("Only grayscale is supported!\n");
    return;
  }
  static size_t hist[HIST_LEVELS];
  static size_t cdf[HIST_LEVELS];
  static size_t T[HIST_LEVELS];
  size_t M = src_img->width * src_img->height;
  memset(hist, 0, HIST_LEVELS * sizeof(size_t));
  memset(cdf, 0, HIST_LEVELS * sizeof(size_t));
  memset(T, 0, HIST_LEVELS * sizeof(size_t));

  for (size_t i = 0; i < src_img->width * src_img->height * src_img->depth; i++) {
    hist[src_img->data.i[i]]++;
  }

  // Calculate CDF of histogram
  calculate_cdf(hist, cdf, HIST_LEVELS);
  size_t cdf_min = cdf[0];

  // Calculate transformed intensity values
  for (size_t i = 0; i < HIST_LEVELS; i++) {
    // Scale the cdf to the range [0, HIST_LEVELS - 1] using a and b, and perform the transformation
    float a = (cdf[i] - cdf_min);
    float b = (M - cdf_min);
    T[i] = floor(a / b * (HIST_LEVELS - 1));
  }

  for (size_t i = 0; i < src_img->width * src_img->height * src_img->depth; i++) {
    dst_img->data.i[i] = (uint8_t)T[src_img->data.i[i]];
  }
}

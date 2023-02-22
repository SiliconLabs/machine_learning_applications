#pragma once

#include <stdio.h>
#include "sl_slist.h"
#include <string.h>
#ifndef UNIT_TEST
#include "sl_iostream_handles.h"
#endif
#include <math.h>
#ifdef __cplusplus
extern "C" {
#endif
enum ImageFormat {IMAGEFORMAT_UINT8, IMAGEFORMAT_FLOAT};
struct Image{
  union DataUnion{
    uint8_t* i;
    float* f;
    void* raw;
  } data;
  enum ImageFormat format;
  size_t width;
  size_t height;
  size_t depth;
};
struct queue_pixel_entry{
  sl_slist_node_t node;
  int x;
  int y;
};
/**
 * @brief Fast clamp function that can compile into only 3(!) instructions. https://stackoverflow.com/a/16659263
 *
 * @param d
 * @param min
 * @param max
 * @return float
 */
float clampf(float d, float min, float max);
/**
 * @brief Get the size of an image format in bytes. E.g. IMAGEFORMAT_FLOAT returns 4 bytes.
 *
 * @param format
 * @return uint8_t
 */
uint8_t sizeof_imageformat(enum ImageFormat format);
/**
 * @brief Convert x,y,z to a 1D index used for accessing pixel values
 *
 * @return size_t
 */
size_t get_index(size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth);
/**
 * @brief Get the total image size in bytes. Respects different image formats.
 *
 * @param img
 * @return size_t
 */
size_t get_image_bytesize(const struct Image *img);
/**
 * @brief Center crop an image to the size of the destination image.
 *
 * @param src_img The source image
 * @param dst_img The destination image
 */
void center_crop(const struct Image* src_img, const struct Image* dst_img);

void generate_random_image(struct Image* out, size_t width, size_t height, size_t depth, enum ImageFormat format);
void generate_empty_image(struct Image* out, size_t width, size_t height, size_t depth, enum ImageFormat format);
/**
 * @brief Finds connected pixels and outputs a label image containing that information.
 *
 * @param dst_label_img The destination label image
 * @param src_img  The source image to run the algorithm on
 * @param threshold The threshold to turn floats into 1s and 0s
 * @param working_memory The working memory for the algorithm
 * @param working_memory_len The size of the working memory for the algorithm
 * @return uint8_t Number of unique blobs of connected pixels
 */
uint8_t find_connected_pixels(const struct Image *dst_label_img, const struct Image *src_img, float threshold, struct queue_pixel_entry* working_memory, uint32_t working_memory_len);
#ifndef UNIT_TEST
inline void export_image(const struct Image *img, const char *title, const char *misc_info, sl_iostream_t *handle)
{
  printf("image:%s,%d,%d,%d,%d,%s\n", title, img->width, img->height, img->depth, (int)img->format, misc_info);
  sl_iostream_write(handle, img->data.raw, img->width * img->height * img->depth * sizeof_imageformat(img->format));
  printf("\n");
}
#endif
void print_image(const struct Image *img);
#ifdef __cplusplus
}
#endif

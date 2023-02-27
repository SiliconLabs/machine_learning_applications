#ifndef SL_VISION_IMAGE_H
#define SL_VISION_IMAGE_H
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
typedef enum  {IMAGEFORMAT_UINT8, IMAGEFORMAT_FLOAT} sl_vision_image_format_t;
typedef struct {
  union data_union{
    uint8_t* i;
    float* f;
    void* raw;
  } data;
  sl_vision_image_format_t format;
  size_t width;
  size_t height;
  size_t depth;
} sl_vision_image_t;

/**
 * @brief Get the size of an image format in bytes. E.g. IMAGEFORMAT_FLOAT returns 4 bytes.
 *
 * @param format
 * @return uint8_t
 */
uint8_t sl_vision_image_format_bytesize(sl_vision_image_format_t format);
/**
 * @brief Convert x,y,z to a 1D index used for accessing pixel values
 *
 * @return size_t
 */
size_t sl_vision_image_index(const sl_vision_image_t* img, size_t x, size_t y, size_t z);
/**
 * @brief Get the total image size in bytes. Respects different image formats.
 *
 * @param img
 * @return size_t
 */
size_t sl_vision_image_bytesize(const sl_vision_image_t *img);
/**
 * @brief Center crop an image to the size of the destination image.
 *
 * @param src_img The source image
 * @param dst_img The destination image
 */
void sl_vision_image_crop_center(const sl_vision_image_t* src_img, const sl_vision_image_t* dst_img);

void sl_vision_image_generate_random(sl_vision_image_t* out, size_t width, size_t height, size_t depth, sl_vision_image_format_t format);
void sl_vision_image_generate_empty(sl_vision_image_t* out, size_t width, size_t height, size_t depth, sl_vision_image_format_t format);
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
uint8_t sl_vision_image_connected_pixels(const sl_vision_image_t *dst_label_img, const sl_vision_image_t *src_img, float threshold);
#ifndef UNIT_TEST
__INLINE void sl_vision_image_export(const sl_vision_image_t *img, const char *title, const char *misc_info, sl_iostream_t *handle)
{
  printf("image:%s,%d,%d,%d,%d,%s\n", title, img->width, img->height, img->depth, (int)img->format, misc_info);
  sl_iostream_write(handle, img->data.raw, img->width * img->height * img->depth * sl_vision_image_format_bytesize(img->format));
  printf("\n");
}
#endif
void sl_vision_image_print(const sl_vision_image_t *img);
#ifdef __cplusplus
}
#endif

#endif // SL_VISION_IMAGE_H

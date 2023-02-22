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
float clampf(float d, float min, float max);
uint8_t sizeof_imageformat(enum ImageFormat format);
size_t get_index(size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth);
size_t get_image_bytesize(const struct Image *img);
void center_crop(const struct Image* src_img, const struct Image* dst_img);
void generate_random_image(struct Image* out, size_t width, size_t height, size_t depth, enum ImageFormat format);
void generate_empty_image(struct Image* out, size_t width, size_t height, size_t depth, enum ImageFormat format);
uint8_t find_connected_pixels(const struct Image *dst_label_img, const struct Image *src_img, float threshold, struct queue_pixel_entry* working_memory, uint32_t working_memory_len);
void normalize_histogram(const struct Image *src_img, const struct Image *dst_img);
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

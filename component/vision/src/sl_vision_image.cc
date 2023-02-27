#include "sl_vision_image.hpp"
#include "math.h"

size_t sl_vision_image_bytesize(const sl_vision_image_t *img)
{
  return img->width * img->height * img->depth * sl_vision_image_format_bytesize(img->format);
}
size_t sl_vision_image_index(const sl_vision_image_t* img, size_t x, size_t y, size_t z)
{
  return z + x * img->depth + img->width * y * img->depth;
}
uint8_t sl_vision_image_format_bytesize(sl_vision_image_format_t format)
{
  switch (format) {
    case IMAGEFORMAT_FLOAT:
      return sizeof(float);
    case IMAGEFORMAT_UINT8:
      return sizeof(uint8_t);
  }
  return 0;
}
void sl_vision_image_crop_center(const sl_vision_image_t* src_img, const sl_vision_image_t* dst_img)
{
  switch (src_img->format) {
    case IMAGEFORMAT_UINT8:
      generic_sl_vision_image_crop_center<uint8_t>(src_img, dst_img);
      break;
    case IMAGEFORMAT_FLOAT:
      generic_sl_vision_image_crop_center<float>(src_img, dst_img);
      break;
    default:
      printf("Unsupported image format in file %s:%d!\n", __FILE__, __LINE__);
      exit(1);
  }
}
void sl_vision_image_generate_random(sl_vision_image_t* out, size_t width, size_t height, size_t depth, sl_vision_image_format_t format)
{
  switch (format) {
    case IMAGEFORMAT_FLOAT:
      generic_sl_vision_image_generate_random<float>(out, width, height, depth);
      break;
    case IMAGEFORMAT_UINT8:
      generic_sl_vision_image_generate_random<uint8_t>(out, width, height, depth);
      break;
    default:
      printf("Unsupported image format in file %s:%d!\n", __FILE__, __LINE__);
      exit(1);
  }
  out->format = format;
}
void sl_vision_image_generate_empty(sl_vision_image_t* out, size_t width, size_t height, size_t depth, sl_vision_image_format_t format)
{
  switch (format) {
    case IMAGEFORMAT_FLOAT:
      generic_sl_vision_image_generate_empty<float>(out, width, height, depth);
      break;
    case IMAGEFORMAT_UINT8:
      generic_sl_vision_image_generate_empty<uint8_t>(out, width, height, depth);
      break;
    default:
      printf("Unsupported image format in file %s:%d!\n", __FILE__, __LINE__);
      exit(1);
  }
  out->format = format;
}

uint8_t sl_vision_image_connected_pixels(const sl_vision_image_t *dst_label_img, const sl_vision_image_t *src_img, float threshold)
{
  switch (src_img->format) {
    case IMAGEFORMAT_FLOAT:
      return generic_sl_vision_image_connected_pixels<float>(dst_label_img, src_img, threshold);
    case IMAGEFORMAT_UINT8:
      return generic_sl_vision_image_connected_pixels<uint8_t>(dst_label_img, src_img, threshold);
    default:
      printf("Unsupported image format in file %s:%d!\n", __FILE__, __LINE__);
      exit(1);
  }
  return 0;
}
void sl_vision_image_print(const sl_vision_image_t *img)
{
  for (size_t y = 0; y < img->height; y++) {
    for (size_t x = 0; x < img->width; x++) {
      switch (img->format) {
        case IMAGEFORMAT_UINT8:
          printf("%i,", generic_sl_vision_image_pixel_get_value<uint8_t>(img, x, y, 0));
          break;
        case IMAGEFORMAT_FLOAT:
          printf("%f,", generic_sl_vision_image_pixel_get_value<float>(img, x, y, 0));
          break;
        default:
          printf("Unsupported image format in file %s:%d!\n", __FILE__, __LINE__);
          exit(1);
      }
    }
    printf("\n");
  }
}

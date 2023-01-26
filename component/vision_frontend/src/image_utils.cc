#include "image_utils.hpp"
#include "math.h"
/**
 * @brief Get the size of an image format in bytes. E.g. IMAGEFORMAT_FLOAT returns 4 bytes.
 *
 * @param format
 * @return uint8_t
 */
uint8_t sizeof_imageformat(enum ImageFormat format)
{
  switch (format) {
    case IMAGEFORMAT_FLOAT:
      return sizeof(float);
    case IMAGEFORMAT_UINT8:
      return sizeof(uint8_t);
  }
  return 0;
}
/**
 * @brief Fast clamp function that can compile into only 3(!) instructions. https://stackoverflow.com/a/16659263
 *
 * @param d
 * @param min
 * @param max
 * @return float
 */
float clampf(float d, float min, float max)
{
  const float t = d < min ? min : d;
  return t > max ? max : t;
}
/**
 * @brief Convert x,y,z to a 1D index used for accessing pixel values
 *
 * @return size_t
 */
size_t get_index(size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth)
{
  return z + x * depth + width * y * depth;
}
/**
 * @brief Get the total image size in bytes. Respects different image formats.
 *
 * @param img
 * @return size_t
 */
size_t get_image_bytesize(const struct Image *img)
{
  return img->width * img->height * img->depth * sizeof_imageformat(img->format);
}
/**
 * @brief Center crop an image
 *
 * @param src_img The source image
 * @param dst_img The destination image
 */
void center_crop(const struct Image* src_img, const struct Image* dst_img)
{
  switch (src_img->format) {
    case IMAGEFORMAT_UINT8:
      _center_crop<uint8_t>(src_img, dst_img);
      break;
    case IMAGEFORMAT_FLOAT:
      _center_crop<float>(src_img, dst_img);
      break;
    default:
      printf("Unsupported image format in file %s:%d!\n", __FILE__, __LINE__);
      exit(1);
  }
}
void generate_random_image(struct Image* out, size_t width, size_t height, size_t depth, enum ImageFormat format)
{
  switch (format) {
    case IMAGEFORMAT_FLOAT:
      _generate_random_image<float>(out, width, height, depth);
      break;
    case IMAGEFORMAT_UINT8:
      _generate_random_image<uint8_t>(out, width, height, depth);
      break;
    default:
      printf("Unsupported image format in file %s:%d!\n", __FILE__, __LINE__);
      exit(1);
  }
  out->format = format;
}
void generate_empty_image(struct Image* out, size_t width, size_t height, size_t depth, enum ImageFormat format)
{
  switch (format) {
    case IMAGEFORMAT_FLOAT:
      _generate_empty_image<float>(out, width, height, depth);
      break;
    case IMAGEFORMAT_UINT8:
      _generate_empty_image<uint8_t>(out, width, height, depth);
      break;
    default:
      printf("Unsupported image format in file %s:%d!\n", __FILE__, __LINE__);
      exit(1);
  }
  out->format = format;
}
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
uint8_t find_connected_pixels(const struct Image *dst_label_img, const struct Image *src_img, float threshold, struct queue_pixel_entry* working_memory, uint32_t working_memory_len)
{
  switch (src_img->format) {
    case IMAGEFORMAT_FLOAT:
      return _find_connected_pixels<float>(dst_label_img, src_img, threshold, working_memory, working_memory_len);
    case IMAGEFORMAT_UINT8:
      return _find_connected_pixels<uint8_t>(dst_label_img, src_img, threshold, working_memory, working_memory_len);
    default:
      printf("Unsupported image format in file %s:%d!\n", __FILE__, __LINE__);
      exit(1);
  }
  return 0;
}
void print_image(const struct Image *img)
{
  for (size_t y = 0; y < img->height; y++) {
    for (size_t x = 0; x < img->width; x++) {
      switch (img->format) {
        case IMAGEFORMAT_UINT8:
          printf("%i,", _get_pixel_value<uint8_t>(img, x, y, 0));
          break;
        case IMAGEFORMAT_FLOAT:
          printf("%f,", _get_pixel_value<float>(img, x, y, 0));
          break;
        default:
          printf("Unsupported image format in file %s:%d!\n", __FILE__, __LINE__);
          exit(1);
      }
    }
    printf("\n");
  }
}

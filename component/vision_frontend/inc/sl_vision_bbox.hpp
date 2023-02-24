#ifndef SL_VISION_BBOX_HPP
#define SL_VISION_BBOX_HPP
#include "sl_vision_bbox.h"
#include "sl_vision_image.hpp"
/**
 * @brief This function box blurs the area of the input image that is covered by the given bounding box.
 *
 */
template <typename T>
void cpp_sl_vision_bbox_blur(const sl_vision_image_t* img, const sl_vision_bbox_t* bb, size_t kernel_size)
{
  size_t start_x = (size_t)bb->x;
  size_t start_y = (size_t)bb->y;
  size_t end_x = (size_t)(bb->x + bb->width);
  size_t end_y = (size_t)(bb->y + bb->height);

  for (size_t x = start_x; x < end_x; x++) {
    for (size_t y = start_y; y < end_y; y++) {
      for (size_t z = 0; z < img->depth; z++) {
        // Do a box blur and apply it to the image
        T blur_value = cpp_sl_vision_image_pixel_blur<T>(img, x, y, z, kernel_size);
        cpp_sl_vision_image_pixel_set_value<T>(img, x, y, z, blur_value);
      }
    }
  }
}
/**
 * @brief This function pixelizes an area on an image given by a bounding box.
 *
 */
template <typename T>
void cpp_sl_vision_bbox_pixelize(const sl_vision_image_t* img, const sl_vision_bbox_t* bb, size_t pixel_size)
{
  size_t start_x = (size_t)bb->x;
  size_t start_y = (size_t)bb->y;
  size_t end_x = (size_t)(bb->x + bb->width);
  size_t end_y = (size_t)(bb->y + bb->height);

  for (size_t x = start_x; x < end_x; x += pixel_size) {
    for (size_t y = start_y; y < end_y; y += pixel_size) {
      for (size_t z = 0; z < img->depth; z++) {
        // Get the average pixel value of the pixelized area
        T pixel_value = cpp_sl_vision_image_pixel_get_value<T>(img, x, y, z);
        for (size_t i = 0; i < pixel_size; i++) {
          for (size_t j = 0; j < pixel_size; j++) {
            cpp_sl_vision_image_pixel_set_value<T>(img, x + i, y + j, z, pixel_value);
          }
        }
      }
    }
  }
}
#endif // SL_VISION_BBOX_HPP

#ifndef IMAGE_UTILS_HPP
#define IMAGE_UTILS_HPP
#include "image_utils.h"

#include <random>
template <typename T>
T _get_pixel_value(const struct Image *img, size_t x, size_t y, size_t z)
{
  return ((T *)img->data.raw)[get_index(x, y, z, img->width, img->height, img->depth)];
}
/**
 * @brief Set the pixel value at the given coordinates
 *
 * @tparam T
 * @param img
 * @param x
 * @param y
 * @param z
 * @param val
 */
template <typename T>
void _set_pixel_value(const struct Image *img, size_t x, size_t y, size_t z, T val)
{
  ((T *)img->data.raw)[get_index(x, y, z, img->width, img->height, img->depth)] = val;
}

/**
   Blur a pixel with a NxN kernel, returns the blurred value of the pixel
   @param img The image to blur
   @param x The x coordinate of the pixel to blur
   @param y The y coordinate of the pixel to blur
   @param z The z coordinate of the pixel to blur
   @param kernel_size The size of the kernel to use
   @return The blurred value of the pixel
 */
template <typename T>
T _blur_pixel(const struct Image *img, size_t x, size_t y, size_t z, size_t kernel_size)
{
  size_t kernel_half_size = kernel_size / 2;
  size_t kernel_area = kernel_size * kernel_size;
  int start_x = x - kernel_half_size;
  int start_y = y - kernel_half_size;
  int end_x = x + kernel_half_size;
  int end_y = y + kernel_half_size;
  T sum = 0;
  for (int i = start_x; i < end_x; i++) {
    for (int j = start_y; j < end_y; j++) {
      if (i < 0 || j < 0) {
        continue;
      }
      if (i >= (int)img->width || j >= (int)img->height) {
        continue;
      }
      T val = _get_pixel_value<T>(img, i, j, z) / kernel_area;
      sum += val;
      // Only happens when the sum overflows, so should never execute for float type
      // If it does execute for float, then output will be wrong...
      if (sum < val) {
        sum = -1;
      }
    }
  }
  return sum;
}
/**
 * @brief Center crop an image
 * @param src_img The image to crop
 * @param dst_img The image to write the cropped image to
 * @param new_width The new width of the image
 * @param new_height The new height of the image
 */
template<typename T>
void _center_crop(const struct Image* src_img, const struct Image* dst_img)
{
  size_t new_width = dst_img->width;
  size_t new_height = dst_img->height;
  size_t start_x = (src_img->width - new_width) / 2;
  size_t start_y = (src_img->height - new_height) / 2;
  size_t end_x = start_x + new_width;
  size_t end_y = start_y + new_height;
  for (size_t x = start_x; x < end_x; x++) {
    for (size_t y = start_y; y < end_y; y++) {
      for (size_t z = 0; z < src_img->depth; z++) {
        T val = _get_pixel_value<T>(src_img, x, y, z);
        _set_pixel_value<T>(dst_img, x - start_x, y - start_y, z, val);
      }
    }
  }
}
template <typename T>
uint8_t _find_connected_pixels(const struct Image *dst_label_img, const struct Image *src_img, float threshold, struct queue_pixel_entry* working_memory, uint32_t working_memory_len)
{
  static int dxs[] = { 0, 1, -1 };
  static int dys[] = { 0, 1, -1 };
  memset(working_memory, 0, working_memory_len * sizeof(queue_pixel_entry));
  uint32_t queue_ptr = 0;
  sl_slist_node_t *head;
  sl_slist_init(&head);

  uint8_t current_label = 0;
  for (size_t y = 0; y < src_img->height; y++) {
    for (size_t x = 0; x < src_img->width; x++) {
      if (_get_pixel_value<T>(src_img, x, y, 0) < threshold) {
        continue;
      }
      if (_get_pixel_value<uint8_t>(dst_label_img, x, y, 0) > 0) {
        continue;
      }
      current_label++;
      _set_pixel_value<uint8_t>(dst_label_img, x, y, 0, current_label);
      working_memory[queue_ptr] = { .x = (int)x, .y = (int)y };
      sl_slist_push_back(&head, &working_memory[queue_ptr].node);
      queue_ptr++;
      while (head != NULL) {
        sl_slist_node* current_node = sl_slist_pop(&head);

        struct queue_pixel_entry *current = SL_SLIST_ENTRY(current_node, struct queue_pixel_entry, node);
        for (int i_dy = 0; i_dy < (int)(sizeof(dys) / sizeof(dys[0])); i_dy++) {
          for (int i_dx = 0; i_dx < (int)(sizeof(dxs) / sizeof(dxs[0])); i_dx++) {
            int dx = dxs[i_dx];
            int dy = dys[i_dy];
            if (dx == 0 && dy == 0) {
              continue;
            }
            int next_x = current->x + dx;
            int next_y = current->y + dy;
            if (next_x < 0 || next_x >= (int)src_img->width) {
              continue;
            }
            if (next_y < 0 || next_y >= (int)src_img->height) {
              continue;
            }
            if (_get_pixel_value<T>(src_img, next_x, next_y, 0) >= threshold && _get_pixel_value<uint8_t>(dst_label_img, next_x, next_y, 0) == 0) {
              _set_pixel_value<uint8_t>(dst_label_img, next_x, next_y, 0, current_label);
              working_memory[queue_ptr] =  {   .x = next_x, .y = next_y };
              sl_slist_push_back(&head, &working_memory[queue_ptr].node);
              queue_ptr++;
            }
          }
        }
      }
    }
  }
  return current_label;
}
/**
 * @brief Generate a random image
 *
 * @param out The output image struct, all the fields will be allocated and properly set
 * @param width
 * @param height
 * @param depth
 */
template<typename T>
void _generate_random_image(struct Image* out, size_t width, size_t height, size_t depth)
{
  out->width = width;
  out->height = height;
  out->depth = depth;
  out->data.raw = malloc(width * height * depth * sizeof(T));

  std::mt19937 rng;
  rng.seed(std::random_device()());
  std::uniform_real_distribution<float> dist(0, 255);
  for (size_t i = 0; i < width * height * depth; i++) {
    ((T *)out->data.raw)[i] = (T)dist(rng);
  }
}
template<typename T>
void _generate_empty_image(struct Image* out, size_t width, size_t height, size_t depth)
{
  out->width = width;
  out->height = height;
  out->depth = depth;
  out->data.raw = malloc(width * height * depth * sizeof(T));
  memset(out->data.raw, 0, width * height * depth * sizeof(T));
}
#endif /* IMAGE_UTILS_HPP */

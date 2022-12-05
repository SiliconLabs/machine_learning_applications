#include <stdio.h>
#include "sl_slist.h"
#include <string.h>
#include "sl_iostream_handles.h"
#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H
#ifdef __cplusplus

extern "C" {
#endif
enum ImageFormat {IMAGEFORMAT_UINT8, IMAGEFORMAT_FLOAT};
typedef float (*process_func_t) (float);
struct Image{
  union DataUnion{
    uint8_t* i;
    float* f;
    void* raw;
  } data;
  enum ImageFormat type;
  size_t width;
  size_t height;
  size_t depth;
};
struct queue_pixel_entry{
  sl_slist_node_t node;
  int x;
  int y;
};
struct queue_bbox_entry{
  sl_slist_node_t node;
  const struct bbox* bb;
};
struct bbox {
  float x;
  float y;
  float width;
  float height;
  uint16_t class_id;
  float confidence;
};
struct centroid{
  float x;
  float y;
  uint16_t count;
  struct centroid* next_centroid;
  float next_centroid_dist_squared;
  struct centroid* prev_centroid;
  float prev_centroid_dist_squared;
};
float clampf(float d, float min, float max);
uint8_t sizeof_imageformat(enum ImageFormat format);
size_t get_index(size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth);
void center_crop(uint8_t *dst_img, size_t new_width, size_t new_height, uint8_t *src_img, size_t width, size_t height);
void scale_nearest_neighbour(uint8_t *dst_img, size_t new_width, size_t new_height, uint8_t *src_img, size_t width, size_t height);
void scale_bilinear(uint8_t *dst_img, size_t new_width, size_t new_height, uint8_t *src_img, size_t width, size_t height);
void process_image(const struct Image *dst_img, const struct Image *src_img, process_func_t process_func);
uint8_t find_connected_pixels(const struct Image *dst_label_img, const struct Image *src_img, float threshold, struct queue_pixel_entry* working_memory, uint32_t working_memory_len);
uint8_t non_max_suppression(struct bbox bboxes[], uint8_t max_num_bboxes, struct bbox bboxes_out[], float iou_threshold);
void find_centroids_connected_pixels(const struct Image *label_img, const struct Image *src_img, const struct Image *target_scale_image, struct centroid centroids_out[], uint8_t num_labels);
void find_centroids_bboxes(struct bbox bboxes[], uint8_t num_bboxes, struct centroid centroids_out[]);
void update_centroid_connections(struct centroid centroids_prev[], uint8_t num_labels_prev, struct centroid centroids_now[], uint8_t num_labels_now, float max_dist);
void export_bboxes_over_serial(const struct bbox bboxes[], uint8_t num_boxes, uint8_t precision);
void export_image(const struct Image *img, const char *title, const char *misc_info, sl_iostream_t *handle);
void export_centroids_over_serial(const struct centroid centroids[], uint8_t num_labels, uint8_t precision);
void print_image(const struct Image *img);
#ifdef __cplusplus
}
#endif
#endif

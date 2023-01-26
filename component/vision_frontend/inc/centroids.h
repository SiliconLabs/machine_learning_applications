#ifndef CENTROIDS_H
#define CENTROIDS_H

#include <stdio.h>
#include "image_utils.h"
#include "bboxes.h"
#ifdef __cplusplus

extern "C" {
#endif

struct centroid{
  float x;
  float y;
  uint16_t count;
  struct centroid* next_centroid;
  float next_centroid_dist_squared;
  struct centroid* prev_centroid;
  float prev_centroid_dist_squared;
};
void find_centroids_connected_pixels(const struct Image *label_img, const struct Image *src_img, struct centroid centroids_out[], uint8_t num_labels);
void find_centroids_bboxes(struct bbox bboxes[], uint8_t num_bboxes, struct centroid centroids_out[]);
void update_centroid_connections(struct centroid centroids_prev[], uint8_t num_labels_prev, struct centroid centroids_now[], uint8_t num_labels_now, float max_dist);
void export_centroids_over_serial(const struct centroid centroids[], uint8_t num_labels, uint8_t precision);

#ifdef __cplusplus
}
#endif

#endif /* CENTROIDS_H */

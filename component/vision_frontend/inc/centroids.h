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
/**
 * @brief Finds centroids from connected pixels represented in a label image
 *
 * @param label_img The label image of the connected pixels
 * @param src_img The source image, same as the one used to calculate the label image.
 * @param raw_img The raw camera image, used to calculate scaling
 * @param centroids_out The centroids output array
 * @param num_labels The number of unique blobs on connected pixels
 */
void find_centroids_connected_pixels(const struct Image *label_img, const struct Image *src_img, struct centroid centroids_out[], uint8_t num_labels);
/**
 * @brief Given a list of bounding boxes, output a list of centroids
 *
 * @param bboxes
 * @param num_bboxes
 * @param centroids_out
 */
void find_centroids_bboxes(struct bbox bboxes[], uint8_t num_bboxes, struct centroid centroids_out[]);
/**
 * @brief Function that keeps track of centroids over time.
 * Based on the assumption that the object closest in the current timestep to an object in the previous timestep, is the same object.
 *
 * @param centroids_prev The centroids in the previous timestep
 * @param num_labels_prev The number of centroids in the previous timestep
 * @param centroids_now The centroids in the current timestep
 * @param num_labels_now The number of centroids in the current timestep
 * @param max_dist Maximum distance to consider objects as the same
 */
void update_centroid_connections(struct centroid centroids_prev[], uint8_t num_labels_prev, struct centroid centroids_now[], uint8_t num_labels_now, float max_dist);
/**
 * @brief Exports the centroids over serial
 *
 * @param centroids
 * @param num_labels
 * @param precision
 */
void export_centroids_over_serial(const struct centroid centroids[], uint8_t num_labels, uint8_t precision);

#ifdef __cplusplus
}
#endif

#endif // CENTROIDS_H

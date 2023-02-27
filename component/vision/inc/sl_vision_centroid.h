#ifndef SL_VISION_CENTROID_H
#define SL_VISION_CENTROID_H

#include <stdio.h>
#include "sl_vision_image.h"
#include "sl_vision_bbox.h"
#ifdef __cplusplus

extern "C" {
#endif

typedef struct sl_vision_centroid{
  float x;
  float y;
  uint16_t count;
  struct sl_vision_centroid* next_centroid;
  float next_centroid_dist_squared;
  struct sl_vision_centroid* prev_centroid;
  float prev_centroid_dist_squared;
} sl_vision_centroid_t;
/**
 * @brief Finds centroids from connected pixels represented in a label image
 *
 * @param label_img The label image of the connected pixels
 * @param src_img The source image, same as the one used to calculate the label image.
 * @param raw_img The raw camera image, used to calculate scaling
 * @param centroids_out The centroids output array
 * @param num_labels The number of unique blobs on connected pixels
 */
void sl_vision_centroid_from_connected_pixels(const sl_vision_image_t *label_img, const sl_vision_image_t *src_img, sl_vision_centroid_t centroids_out[], uint8_t num_labels);
/**
 * @brief Given a list of bounding boxes, output a list of centroids
 *
 * @param bboxes
 * @param num_bboxes
 * @param centroids_out
 */
void sl_vision_centroid_from_bboxes(sl_vision_bbox_t bboxes[], uint8_t num_bboxes, sl_vision_centroid_t centroids_out[]);
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
void sl_vision_centroid_track(sl_vision_centroid_t centroids_prev[], uint8_t num_labels_prev, sl_vision_centroid_t centroids_now[], uint8_t num_labels_now, float max_dist);
/**
 * @brief Exports the centroids over serial
 *
 * @param centroids
 * @param num_labels
 * @param precision
 */
void sl_vision_centroid_export_over_serial(const sl_vision_centroid_t centroids[], uint8_t num_labels, uint8_t precision);

#ifdef __cplusplus
}
#endif

#endif // SL_VISION_CENTROID_H

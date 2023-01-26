#include "centroids.h"
#include "image_utils.hpp"
#include "centroids.hpp"
/**
 * @brief Finds centroids from connected pixels represented in a label image
 *
 * @param label_img The label image of the connected pixels
 * @param src_img The source image, same as the one used to calculate the label image.
 * @param raw_img The raw camera image, used to calculate scaling
 * @param centroids_out The centroids output array
 * @param num_labels The number of unique blobs on connected pixels
 */
void find_centroids_connected_pixels(const struct Image *label_img, const struct Image *src_img, struct centroid centroids_out[], uint8_t num_labels)
{
  switch (src_img->format) {
    case IMAGEFORMAT_FLOAT:
      return _find_centroids_connected_pixels<float>(label_img, src_img, centroids_out, num_labels);
    case IMAGEFORMAT_UINT8:
      return _find_centroids_connected_pixels<uint8_t>(label_img, src_img, centroids_out, num_labels);
  }
}
/**
 * @brief Given a list of bounding boxes, output a list of centroids
 *
 * @param bboxes
 * @param num_bboxes
 * @param centroids_out
 */
void find_centroids_bboxes(struct bbox bboxes[], uint8_t num_bboxes, struct centroid centroids_out[])
{
  for (uint8_t i = 0; i < num_bboxes; i++) {
    float cx = bboxes[i].x + bboxes[i].width / 2;
    float cy = bboxes[i].y + bboxes[i].height / 2;
    uint16_t count = (int)(bboxes[i].width * bboxes[i].height);
    centroids_out[i] = { .x = cx, .y = cy, .count = count };
  }
}
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
void update_centroid_connections(struct centroid centroids_prev[], uint8_t num_labels_prev, struct centroid centroids_now[], uint8_t num_labels_now, float max_dist)
{
  float squared_max_dist = max_dist * max_dist;
  for (uint8_t label_id_now = 0; label_id_now < num_labels_now; label_id_now++) {
    struct centroid *centroid_now = &centroids_now[label_id_now];
    centroid_now->prev_centroid = NULL;
    centroid_now->prev_centroid_dist_squared = -1;
    for (uint8_t label_id_prev = 0; label_id_prev < num_labels_prev; label_id_prev++) {
      struct centroid *centroid_prev = &centroids_prev[label_id_prev];
      float delta_x = centroid_now->x - centroid_prev->x;
      float delta_y = centroid_now->y - centroid_prev->y;
      float squared_dist = (delta_x * delta_x) + (delta_y * delta_y);
      if (max_dist != -1 && squared_dist > squared_max_dist) {
        continue;
      }
      if (centroid_now->prev_centroid_dist_squared == -1 || squared_dist < centroid_now->prev_centroid_dist_squared) {
        // If the previous centroid has a next centroid
        if (centroid_prev->next_centroid != NULL) {
          // If it has a better distance measure than ours, skip
          if (centroid_prev->next_centroid_dist_squared < squared_dist) {
            continue;
          }
          // If this centroid is closer, we disconnect any other current centroid to that is currently connected to the previous centroid
          centroid_prev->next_centroid->prev_centroid = NULL;
          centroid_prev->next_centroid->prev_centroid_dist_squared = -1;
        }

        // We add a new connection to the current centroid we are checking
        centroid_now->prev_centroid = centroid_prev;
        centroid_now->prev_centroid_dist_squared = squared_dist;

        centroid_prev->next_centroid = centroid_now;
        centroid_prev->next_centroid_dist_squared = squared_dist;
      }
    }
  }
}
/**
 * @brief Exports the centroids over serial
 *
 * @param centroids
 * @param num_labels
 * @param precision
 */
void export_centroids_over_serial(const struct centroid centroids[], uint8_t num_labels, uint8_t precision)
{
  printf("centroids:%i\n", num_labels);
  for (uint8_t i = 0; i < num_labels; i++) {
    if (centroids[i].prev_centroid == NULL) {
      printf("%.*f,%.*f,%d\n",
             precision, centroids[i].x,
             precision, centroids[i].y,
             centroids[i].count
             );
    } else {
      printf("%.*f,%.*f,%d-%.*f,%.*f,%d,%.*f\n",
             precision, centroids[i].x,
             precision, centroids[i].y,
             centroids[i].count,
             precision, centroids[i].prev_centroid->x,
             precision, centroids[i].prev_centroid->y,
             centroids[i].prev_centroid->count,
             precision, centroids[i].prev_centroid_dist_squared
             );
    }
  }
}

#include "sl_vision_centroid.h"
#include "sl_vision_image.hpp"
#include "sl_vision_centroid.hpp"

void sl_vision_centroid_from_connected_pixels(const sl_vision_image_t *label_img, const sl_vision_image_t *src_img, sl_vision_centroid_t centroids_out[], uint8_t num_labels)
{
  switch (src_img->format) {
    case IMAGEFORMAT_FLOAT:
      return cpp_sl_vision_centroid_from_connected_pixels<float>(label_img, src_img, centroids_out, num_labels);
    case IMAGEFORMAT_UINT8:
      return cpp_sl_vision_centroid_from_connected_pixels<uint8_t>(label_img, src_img, centroids_out, num_labels);
  }
}

void sl_vision_centroid_from_bboxes(sl_vision_bbox_t bboxes[], uint8_t num_bboxes, sl_vision_centroid_t centroids_out[])
{
  for (uint8_t i = 0; i < num_bboxes; i++) {
    float cx = bboxes[i].x + bboxes[i].width / 2;
    float cy = bboxes[i].y + bboxes[i].height / 2;
    uint16_t count = (int)(bboxes[i].width * bboxes[i].height);
    centroids_out[i] = { .x = cx, .y = cy, .count = count };
  }
}

void sl_vision_centroid_track(sl_vision_centroid_t centroids_prev[], uint8_t num_labels_prev, sl_vision_centroid_t centroids_now[], uint8_t num_labels_now, float max_dist)
{
  float squared_max_dist = max_dist * max_dist;
  for (uint8_t label_id_now = 0; label_id_now < num_labels_now; label_id_now++) {
    sl_vision_centroid_t *centroid_now = &centroids_now[label_id_now];
    centroid_now->prev_centroid = NULL;
    centroid_now->prev_centroid_dist_squared = -1;
    for (uint8_t label_id_prev = 0; label_id_prev < num_labels_prev; label_id_prev++) {
      sl_vision_centroid_t *centroid_prev = &centroids_prev[label_id_prev];
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

void sl_vision_centroid_export_over_serial(const sl_vision_centroid_t centroids[], uint8_t num_labels, uint8_t precision)
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

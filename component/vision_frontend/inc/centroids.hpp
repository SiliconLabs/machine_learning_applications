#ifndef CENTROIDS_HPP
#define CENTROIDS_HPP

#include "image_utils.h"

template <typename T>
void _find_centroids_connected_pixels(const struct Image *label_img, const struct Image *src_img, struct centroid centroids_out[], uint8_t num_labels)
{
  uint16_t counts[num_labels];
  float x_vals[num_labels];
  float y_vals[num_labels];

  memset(counts, 0, num_labels * sizeof(uint16_t));
  memset(x_vals, 0, num_labels * sizeof(float));
  memset(y_vals, 0, num_labels * sizeof(float));

  for (size_t y = 0; y < label_img->height; y++) {
    for (size_t x = 0; x < label_img->width; x++) {
      uint8_t label = _get_pixel_value<uint8_t>(label_img, x, y, 0);
      if (label == 0) {
        continue;
      }
      counts[label - 1]++;
      T val = _get_pixel_value<T>(src_img, x, y, 0);
      // Sum the coordinates of a label
      x_vals[label - 1] += ((float)x + 0.5f);
      y_vals[label - 1] += ((float)y + 0.5f);
    }
  }
  for (uint8_t label_id = 0; label_id < num_labels; label_id++) {
    // Take the average of the coordinate sums to get the centroid
    centroids_out[label_id].x = (x_vals[label_id] / counts[label_id]);
    centroids_out[label_id].y = (y_vals[label_id] / counts[label_id]);
    centroids_out[label_id].count = counts[label_id];
  }
}
#endif /* CENTROIDS_HPP */

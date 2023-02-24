#ifndef SL_VISION_H
#define SL_VISION_H
#include "sl_vision_image.h"
#include "sl_vision_bbox.h"
#include "sl_vision_centroid.h"
#include "sl_vision_histogram.h"
/**
 * @brief Fast clamp function that can compile into only 3(!) instructions. https://stackoverflow.com/a/16659263
 *
 * @param d
 * @param min
 * @param max
 * @return float
 */
float sl_vision_clampf(float d, float min, float max)
{
  const float t = d < min ? min : d;
  return t > max ? max : t;
}
#endif // SL_VISION_H

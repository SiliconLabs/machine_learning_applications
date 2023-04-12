/***************************************************************************//**
 * @file
 * @brief HTM Implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef SL_HTM_UTILS_H
#define SL_HTM_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
/**
 * @brief Fast clamp function that can compile into only 3(!) instructions. https://stackoverflow.com/a/16659263
 *
 * @param d
 * @param min
 * @param max
 * @return float
 */
uint8_t sl_htm_utils_clamp_uint8(uint8_t d, uint8_t min, uint8_t max);
int sl_htm_utils_floorf(float x);
uint16_t sl_htm_utils_xy_to_index(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
uint8_t sl_htm_utils_index_to_x(uint16_t index, uint8_t width);
uint8_t sl_htm_utils_index_to_y(uint16_t index, uint8_t width, uint8_t height);
#ifdef __cplusplus
}
#endif

#endif // SL_HTM_UTILS_H

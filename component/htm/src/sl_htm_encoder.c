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
#include "sl_htm_encoder.h"
#include "sl_htm_utils.h"

void sl_htm_encoder_simple_number(float value, float min_value, float max_value, uint16_t num_active_bits, sl_htm_sdr_t* output_sdr)
{
  float range = max_value - min_value;
  uint16_t n = output_sdr->width * output_sdr->height;
  uint16_t num_buckets = n - num_active_bits + 1;

  sl_htm_sdr_clear(output_sdr);
  uint16_t index = sl_htm_utils_floorf((float)num_buckets * (value - min_value) / range);
  for (uint16_t i = 0; i < num_active_bits; i++) {
    sl_htm_sdr_set_bit(output_sdr, index + i, 1);
  }
}

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
#ifndef SL_HTM_ENCODER_H
#define SL_HTM_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_htm_sdr.h"
/**
 * @brief Encodes a numeric value into an SDR
 *
 * @param value The value to encode
 * @param min_value The minimum value that can be encoded
 * @param max_value  The maximum value that can be encoded
 * @param num_active_bits The number of active bits that you want the value to be encoded into
 * @param output_sdr The SDR instance to encode the value into
 */
void sl_htm_encoder_simple_number(float value, float min_value, float max_value, uint16_t num_active_bits, sl_htm_sdr_t* output_sdr);

#ifdef __cplusplus
}
#endif

#endif // SL_HTM_ENCODER_H

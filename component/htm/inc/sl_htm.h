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
#ifndef SL_HTM_H
#define SL_HTM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_htm_sdr.h"
#include "sl_htm_sp.h"
#include "sl_htm_tm.h"
#include "sl_htm_encoder.h"
#include "sl_htm_utils.h"
/**
 * @brief Initialize the HTM system. This will initialize the SP and TM using the provided parameters.
 *
 * @param input_width Width of the input SDR
 * @param input_height Height of the input SDR
 * @param width Width of the SP output and TM input
 * @param height Height of the SP output and TM input
 * @param params_sp Parameters for the Spatial Pooler
 * @param params_tm Parameters for the Temporal Memory
 */
void sl_htm_init(uint16_t input_width, uint16_t input_height, uint16_t width, uint16_t height, sl_htm_sp_parameters_t params_sp, sl_htm_tm_parameters_t params_tm);
/**
 * @brief Execute the HTM system. This will execute the SP and TM using the provided input SDR.
 *
 * @param input_sdr Input SDR
 * @param learn Whether to learn or not
 * @return The anomaly score as a float in range [0, 1]
 */
float sl_htm_execute(sl_htm_sdr_t* input_sdr, bool learn);

sl_htm_sp_t* sl_htm_get_sp();
sl_htm_tm_t* sl_htm_get_tm();

#ifdef __cplusplus
}
#endif

#endif // SL_HTM_H

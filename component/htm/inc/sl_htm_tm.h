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
#ifndef SL_HTM_TM_H
#define SL_HTM_TM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_htm_tm_types.h"
/**
 * @brief Initialize the parameters for the Temporal Memory with default values.
 *
 * @param params
 */
void sl_htm_tm_init_default_params(sl_htm_tm_parameters_t* params);
/**
 * @brief Initialize the Temporal Memory.
 *
 * @param tm The TM instance to initialize
 * @param width Width of the input SDR from the Spatial Pooler
 * @param height Height of the input SDR from the Spatial Pooler
 */
void sl_htm_tm_init(sl_htm_tm_t* tm, uint16_t width, uint16_t height);
/**
 * @brief Execute the Temporal Memory.
 *
 * @param tm The TM instance to execute
 * @param sp_sdr The SDR output from the Spatial Pooler
 * @param learn Whether to learn or not
 * @return The anomaly score as a float in range [0, 1]
 */
float sl_htm_tm_execute(sl_htm_tm_t* tm, sl_htm_sdr_t* sp_sdr, bool learn);
size_t sl_htm_tm_memory_size(sl_htm_tm_t* tm);
#ifdef __cplusplus
}
#endif

#endif // SL_HTM_TM_H

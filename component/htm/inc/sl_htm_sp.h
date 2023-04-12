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
#ifndef SL_HTM_SP_H
#define SL_HTM_SP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_htm_sdr.h"

typedef struct {
  uint8_t sdr_x;
  uint8_t sdr_y;
  uint8_t sp_x;
  uint8_t sp_y;
  uint8_t permanence;
} sl_htm_sp_connection_t;

typedef struct {
  sl_htm_sp_connection_t* connections;
  uint16_t num_connections;
  uint16_t overlap_score;
  uint8_t column_x;
  uint8_t column_y;
} sl_htm_sp_column_t;

typedef struct {
  float sparsity;
  uint8_t permanence_threshold;
  uint8_t permanence_increment;
  uint8_t permanence_decrement;
  uint16_t overlap_score_threshold;
  uint16_t potential_radius;
  float potential_pct;
} sl_htm_sp_parameters_t;

typedef struct {
  sl_htm_sp_parameters_t parameters;
  sl_htm_sp_column_t* columns;
  uint8_t width;
  uint8_t height;
} sl_htm_sp_t;
/**
 * @brief Initialize the spatial pooler using its parameters.
 *
 * @param sp The SP instance to initialize.
 * @param input_width Width of the input SDR
 * @param input_height Height of the input SDR
 * @param output_width Width of the output SDR
 * @param output_height Height of the output SDR
 */
void sl_htm_sp_init(sl_htm_sp_t* sp, uint8_t input_width, uint8_t input_height, uint8_t output_width, uint8_t output_height);
/**
 * @brief Initialize the parameters for the Spatial Pooler with default values.
 *
 * @param params The parameters to initialize.
 */
void sl_htm_sp_init_default_params(sl_htm_sp_parameters_t* params);
/**
 * @brief Execute the spatial pooler.
 *
 * @param sp The SP instance to execute.
 * @param input_sdr The input SDR.
 * @param output_sdr The output SDR.
 * @param learn Whether to learn or not.
 */
void sl_htm_sp_execute(sl_htm_sp_t* sp, sl_htm_sdr_t* input_sdr, sl_htm_sdr_t* output_sdr, bool learn);
/**
 * @brief Print the state of the spatial pooler. It will be a table with the active columns and their overlap scores.
 *
 * @param sp
 */
void sl_htm_sp_print(sl_htm_sp_t* sp);
/**
 * @brief Estimate the memory size of the spatial pooler.
 *
 * @param sp The SP instance to estimate the memory size.
 * @return The memory size in bytes.
 */
size_t sl_htm_sp_memory_size(sl_htm_sp_t* sp);
#ifdef __cplusplus
}
#endif

#endif // SL_HTM_SP_H

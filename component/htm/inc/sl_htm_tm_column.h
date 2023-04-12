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
#ifndef SL_HTM_TM_COLUMN_H
#define SL_HTM_TM_COLUMN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_htm_tm_types.h"

#include "sl_htm_tm_column.h"

void sl_htm_tm_column_init(sl_htm_tm_t* tm, sl_htm_tm_column_t* column);
/**
 * @brief Find the least used cell in the column. The least used cell is the one with the lowest number of segments.
 * If there are multiple cells with the same number of segments, choose one randomly.
 *
 */
sl_htm_tm_cell_t* sl_htm_tm_column_least_used_cell(sl_htm_tm_t* tm, sl_htm_tm_column_t* column);
void sl_htm_tm_column_burst(sl_htm_tm_t* tm, sl_htm_tm_column_t* column, bool learn, sl_htm_tm_state_t* state_current, sl_htm_tm_state_t* state_prev);
bool sl_htm_tm_column_activate_predicted(sl_htm_tm_t* tm, sl_htm_tm_column_t* column, bool learn, sl_htm_tm_state_t* state_current, sl_htm_tm_state_t* state_prev);
void sl_htm_tm_column_punish_predicted(sl_htm_tm_t* tm, sl_htm_tm_column_t* column, sl_htm_tm_state_t* state_prev);
#ifdef __cplusplus
}
#endif

#endif // SL_HTM_TM_COLUMN_H

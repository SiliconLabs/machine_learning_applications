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
#ifndef SL_HTM_TM_CELL_H
#define SL_HTM_TM_CELL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_htm_tm_types.h"

void sl_htm_tm_cell_init(sl_htm_tm_t* tm, sl_htm_tm_cell_t* cell, sl_htm_tm_column_t* parent_column);
sl_htm_tm_segment_t* sl_htm_tm_cell_grow_segment(sl_htm_tm_t * tm, sl_htm_tm_cell_t * cell);
void sl_htm_tm_cell_delete_segment(sl_htm_tm_t* tm, sl_htm_tm_cell_t* cell, sl_htm_tm_segment_t* segment);

#ifdef __cplusplus
}
#endif

#endif // SL_HTM_TM_CELL_H

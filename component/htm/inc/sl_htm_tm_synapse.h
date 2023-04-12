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
#ifndef SL_HTM_TM_SYNAPSE_H
#define SL_HTM_TM_SYNAPSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_htm_tm_types.h"
void sl_htm_tm_synapse_setup(sl_htm_tm_t* tm, sl_htm_tm_synapse_t* synapse, sl_htm_tm_segment_t* parent_segment, sl_htm_tm_cell_t* target_cell);
void sl_htm_tm_synapse_reset(sl_htm_tm_synapse_t* synapse);
bool sl_htm_tm_synapse_is_existing(sl_htm_tm_synapse_t* synapse);
bool sl_htm_tm_synapse_is_existing_active(sl_htm_tm_t* tm, sl_htm_tm_synapse_t* synapse, sl_htm_tm_state_t* state_current);
bool sl_htm_tm_synapse_is_connected_active(sl_htm_tm_t* tm, sl_htm_tm_synapse_t* synapse, sl_htm_tm_state_t* state_current);

#ifdef __cplusplus
}
#endif

#endif // SL_HTM_TM_SYNAPSE_H

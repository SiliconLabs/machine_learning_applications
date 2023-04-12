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
#ifndef SL_HTM_TM_SEGMENT_H
#define SL_HTM_TM_SEGMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_htm_tm_types.h"
#include "sl_htm_tm_segment.h"
#include "sl_htm_tm_synapse.h"

void sl_htm_segment_init(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment);
void sl_htm_segment_reset(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment);
bool sl_htm_tm_segment_is_existing(sl_htm_tm_segment_t* segment);

/**
 * @brief Check if a segment is active, i.e. if it has enough active synapses.
 *
 * @param tm
 * @param segment
 * @return true if num_active_synapses >= activation_threshold
 * @return false otherwise
 */
bool sl_htm_tm_segment_is_active(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment, sl_htm_tm_state_t* state_current);

/**
 * @brief Count the number of active potential synapses in a segment.
 *
 * @param tm
 * @param segment
 * @return uint16_t
 */
uint16_t sl_htm_tm_segment_potential_score(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment, sl_htm_tm_state_t* state);
sl_htm_tm_synapse_t* sl_htm_tm_segment_grow_synapse(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment, sl_htm_tm_cell_t* target_cell);
void sl_htm_tm_segment_delete_synapse(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment, sl_htm_tm_synapse_t* synapse);
void sl_htm_tm_segment_update_permanence(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment, sl_htm_tm_state_t* state_prev);
/**
 * @brief Grow a segment by adding synapses to it or there are no more unconnected winner cells to connect to.
 *
 * @param tm
 * @param segment
 */
void sl_htm_tm_segment_grow_synapses(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment, sl_htm_tm_state_t* state_prev);

#ifdef __cplusplus
}
#endif

#endif // SL_HTM_TM_SEGMENT_H

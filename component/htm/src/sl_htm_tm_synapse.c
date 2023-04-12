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
#include "sl_htm_tm_synapse.h"

void sl_htm_tm_synapse_setup(sl_htm_tm_t* tm, sl_htm_tm_synapse_t* synapse, sl_htm_tm_segment_t* parent_segment, sl_htm_tm_cell_t* target_cell)
{
  synapse->parent_segment = parent_segment;
  synapse->target_cell = target_cell;
  synapse->permanence = tm->parameters.synapse_permanence_initial;
}
void sl_htm_tm_synapse_reset(sl_htm_tm_synapse_t* synapse)
{
  synapse->parent_segment = NULL;
  synapse->target_cell = NULL;
  synapse->permanence = 0;
}
bool sl_htm_tm_synapse_is_existing(sl_htm_tm_synapse_t* synapse)
{
  return synapse->parent_segment != NULL;
}
bool sl_htm_tm_synapse_is_existing_active(sl_htm_tm_t* tm, sl_htm_tm_synapse_t* synapse, sl_htm_tm_state_t* state_current)
{
  return sl_htm_tm_synapse_is_existing(synapse) && sl_htm_tm_is_cell_in_array(synapse->target_cell, &state_current->active_cells);
}
bool sl_htm_tm_synapse_is_connected_active(sl_htm_tm_t* tm, sl_htm_tm_synapse_t* synapse, sl_htm_tm_state_t* state_current)
{
  bool is_existing = sl_htm_tm_synapse_is_existing(synapse);
  bool is_connected = synapse->permanence >= tm->parameters.synapse_permanence_threshold;
  bool is_active = sl_htm_tm_is_cell_in_array(synapse->target_cell, &state_current->active_cells);
  return is_existing && is_connected && is_active;
}

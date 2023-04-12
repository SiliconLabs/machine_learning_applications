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
#include "sl_htm_tm_segment.h"
#include "sl_htm_tm_synapse.h"
#include "sl_htm_tm_cell.h"
void sl_htm_segment_init(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment)
{
  segment->synapses = calloc(tm->parameters.max_synapses_in_segment, sizeof(sl_htm_tm_synapse_t));
  if (segment->synapses == NULL) {
    printf("Error [%s:%d]: Could not allocate memory for synapses.\n", __FILE__, __LINE__);
    while (1);
  }
  segment->num_synapses = 0;
  segment->parent_cell = NULL;
}
void sl_htm_segment_reset(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment)
{
  segment->num_synapses = 0;
  segment->parent_cell = NULL;
  segment->synapses = memset(segment->synapses, 0, sizeof(sl_htm_tm_synapse_t) * tm->parameters.max_synapses_in_segment);
  if (segment->synapses == NULL) {
    printf("Error [%s:%d]: Could not allocate memory for synapses.\n", __FILE__, __LINE__);
    while (1);
  }
}
bool sl_htm_tm_segment_is_existing(sl_htm_tm_segment_t* segment)
{
  return segment->parent_cell != NULL;
}

bool sl_htm_tm_segment_is_active(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment, sl_htm_tm_state_t* state_current)
{
  // Count the number of active synapses
  uint16_t num_active_synapses = 0;
  for (uint16_t i = 0; i < tm->parameters.max_synapses_in_segment; i++) {
    if (sl_htm_tm_synapse_is_connected_active(tm, &segment->synapses[i], state_current)) {
      num_active_synapses++;
    }
  }
  return num_active_synapses >= tm->parameters.segment_activation_threshold;
}

uint16_t sl_htm_tm_segment_potential_score(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment, sl_htm_tm_state_t* state)
{
  uint16_t num_active_potential_synapses = 0;
  for (uint16_t i = 0; i < tm->parameters.max_synapses_in_segment; i++) {
    if (sl_htm_tm_synapse_is_existing_active(tm, &segment->synapses[i], state)) {
      num_active_potential_synapses++;
    }
  }
  return num_active_potential_synapses;
}
/**
 * @brief Grow a new synapse on the segment.
 *
 * @param tm
 * @param segment
 * @param target_cell
 * @return Returns a pointer to the new synapse, or NULL if the segment is full.
 */
sl_htm_tm_synapse_t* sl_htm_tm_segment_grow_synapse(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment, sl_htm_tm_cell_t* target_cell)
{
  //If all the slots are full, return NULL
  if (segment->num_synapses >= tm->parameters.max_synapses_in_segment) {
    return NULL;
  }
  // Find an empty synapse slot
  for (uint16_t i = 0; i < tm->parameters.max_synapses_in_segment; i++) {
    sl_htm_tm_synapse_t* synapse = &segment->synapses[i];
    if (!sl_htm_tm_synapse_is_existing(synapse)) {
      // Grow (initialize) the synapse
      sl_htm_tm_synapse_setup(tm, synapse, segment, target_cell);
      segment->num_synapses++;
      return synapse;
    }
  }
  return NULL;
}
void sl_htm_tm_segment_delete_synapse(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment, sl_htm_tm_synapse_t* synapse)
{
  sl_htm_tm_synapse_reset(synapse);

  segment->num_synapses--;

  // If the segment is empty, delete it
  if (segment->num_synapses == 0) {
    sl_htm_tm_cell_delete_segment(tm, segment->parent_cell, segment);
  }
}
void sl_htm_tm_segment_update_permanence(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment, sl_htm_tm_state_t* state_prev)
{
  for (uint16_t i = 0; i < tm->parameters.max_synapses_in_segment; i++) {
    sl_htm_tm_synapse_t* synapse = &segment->synapses[i];
    if (!sl_htm_tm_synapse_is_existing(synapse)) {
      continue;
    }

    if (sl_htm_tm_is_cell_in_array(synapse->target_cell, &state_prev->active_cells)) {
      if (synapse->permanence < UINT8_MAX - tm->parameters.synapse_permanence_increment) {
        synapse->permanence += tm->parameters.synapse_permanence_increment;
      } else {
        synapse->permanence = UINT8_MAX;
      }
    } else {
      if (synapse->permanence > tm->parameters.synapse_permanence_decrement) {
        synapse->permanence -= tm->parameters.synapse_permanence_decrement;
      } else {
        sl_htm_tm_segment_delete_synapse(tm, segment, synapse);
      }
    }
  }
}

void sl_htm_tm_segment_grow_synapses(sl_htm_tm_t* tm, sl_htm_tm_segment_t* segment, sl_htm_tm_state_t* state_prev)
{
  // Shuffle the previous winner cells
  sl_htm_tm_shuffle_cell_array(&state_prev->active_cells);
  // Go through the previous winner cells
  for (uint16_t i = 0; i < state_prev->active_cells.len; i++) {
    sl_htm_tm_cell_t* cell = state_prev->active_cells.cells[i];
    bool already_connected = false;
    // Check if the cell is already connected to the segment
    for (uint16_t j = 0; j < tm->parameters.max_synapses_in_segment; j++) {
      sl_htm_tm_synapse_t* synapse = &segment->synapses[j];
      if (sl_htm_tm_synapse_is_existing(synapse) && synapse->target_cell == cell) {
        already_connected = true;
        break;
      }
    }
    // If the cell is not already connected, add a synapse to it
    if (!already_connected) {
      sl_htm_tm_synapse_t* synapse = sl_htm_tm_segment_grow_synapse(tm, segment, cell);
      // If the segment is now full, stop growing
      if (synapse == NULL) {
        break;
      }
    }
  }
}

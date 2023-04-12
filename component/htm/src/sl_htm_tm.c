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
#include "sl_htm_tm.h"
#include "sl_htm_tm_synapse.h"
#include "sl_htm_tm_segment.h"
#include "sl_htm_tm_column.h"
#include "sl_htm_sdr.h"
#include "sl_htm_utils.h"
static sl_htm_tm_state_t state_current;
static sl_htm_tm_state_t state_prev;
void sl_htm_tm_init(sl_htm_tm_t* tm, uint16_t width, uint16_t height)
{
  sl_htm_tm_init_state(&state_current);
  sl_htm_tm_init_state(&state_prev);

  tm->width = width;
  tm->height = height;
  tm->columns = malloc(sizeof(sl_htm_tm_column_t) * tm->width * tm->height);
  if (tm->columns == NULL) {
    printf("Error [%s:%d]: Could not allocate memory for columns.\n", __FILE__, __LINE__);
    while (1);
  }
  // Initialize the columns
  for (uint16_t i = 0; i < tm->width * tm->height; i++) {
    sl_htm_tm_column_init(tm, &tm->columns[i]);
  }
}

void sl_htm_tm_init_default_params(sl_htm_tm_parameters_t* params)
{
  params->synapse_permanence_increment = 15;
  params->synapse_permanence_decrement = 15;
  params->synapse_permanence_initial = 64;
  params->synapse_permanence_threshold = 128;

  params->max_segments_in_cell = 6;
  params->max_synapses_in_segment = 4;

  params->num_cells_per_column = 4;

  params->segment_activation_threshold = (uint16_t)(params->max_synapses_in_segment * 0.5f);
  params->segment_learning_threshold = (uint16_t)(params->max_synapses_in_segment * 0.25f);
}

void sl_htm_tm_activate_cells(sl_htm_tm_t* tm, sl_htm_sdr_t* sdr, bool learn, sl_htm_tm_state_t* state_current, sl_htm_tm_state_t* state_prev)
{
  uint16_t num_columns = tm->width * tm->height;
  // Go through all the columns
  for (uint16_t i = 0; i < num_columns; i++) {
    sl_htm_tm_column_t* column = &tm->columns[i];
    bool active = sl_htm_sdr_get_bit(sdr, i);
    if (active) {
      bool any_predictive_cells = sl_htm_tm_column_activate_predicted(tm, column, learn, state_current, state_prev);
      if (!any_predictive_cells) {
        // If no cells are predictive, burst the column
        sl_htm_tm_column_burst(tm, column, learn, state_current, state_prev);
      }
    } else {
      // Punish active segments on columns that are not active
      if (learn) {
        sl_htm_tm_column_punish_predicted(tm, column, state_prev);
      }
    }
  }
}
void sl_htm_tm_activate_dendrites(sl_htm_tm_t* tm, sl_htm_tm_state_t* state_current)
{
  // Go through all columns
  for (uint16_t i = 0; i < tm->width * tm->height; i++) {
    sl_htm_tm_column_t* column = &tm->columns[i];
    // Go through all cells
    for (uint16_t j = 0; j < tm->parameters.num_cells_per_column; j++) {
      sl_htm_tm_cell_t* cell = &column->cells[j];
      // Go through all segments
      for (uint16_t k = 0; k < cell->num_segments; k++) {
        sl_htm_tm_segment_t* segment = &cell->segments[k];
        uint16_t num_active_connected = 0;
        uint16_t num_active_potential = 0;
        // Go through all synapses
        for (uint16_t l = 0; l < segment->num_synapses; l++) {
          sl_htm_tm_synapse_t* synapse = &segment->synapses[l];
          if (sl_htm_tm_is_cell_in_array(synapse->target_cell, &state_current->active_cells)) {
            if (synapse->permanence > 0) {
              num_active_potential++;
            }
            if (synapse->permanence >= tm->parameters.synapse_permanence_threshold) {
              num_active_connected++;
            }
          }
        }
        if (num_active_connected >= tm->parameters.segment_activation_threshold) {
          sl_htm_tm_add_segment_to_array(segment, &state_current->active_segments);
        }
        if (num_active_potential >= tm->parameters.segment_learning_threshold) {
          sl_htm_tm_add_segment_to_array(segment, &state_current->matching_segments);
        }
      }
    }
  }
}
/**
 * @brief Calculate the anomaly score. The anomaly score is the percentage of active columns that were not predicted.
 *
 * @return float
 */
float sl_htm_tm_anomaly_score(sl_htm_sdr_t * sp_sdr, sl_htm_tm_state_t* state_current)
{
  uint16_t both = state_current->num_predictive_and_active_columns;
  uint16_t active = sp_sdr->num_active_bits;
  return (float)(active - both) / (float)active;
}

float sl_htm_tm_execute(sl_htm_tm_t* tm, sl_htm_sdr_t* sp_sdr, bool learn)
{
  // Perform the temporal memory algorithm
  sl_htm_tm_activate_cells(tm, sp_sdr, learn, &state_current, &state_prev);
  sl_htm_tm_activate_dendrites(tm, &state_current);

  // Calculate the anomaly score
  float anomaly_score = sl_htm_tm_anomaly_score(sp_sdr, &state_current);

  // Put the current state into the previous state, and clear the current state
  sl_htm_tm_swap_and_clear_states(&state_prev, &state_current);

  return anomaly_score;
}

size_t sl_htm_tm_memory_size(sl_htm_tm_t* tm)
{
  size_t size = 0;
  // This is the dynamic memory size of the temporal memory
  // size += sizeof(sl_htm_tm_t);
  // size += sizeof(sl_htm_tm_column_t) * tm->width * tm->height;
  // // Go through all the columns
  // for (uint16_t i = 0; i < tm->width * tm->height; i++) {
  //   sl_htm_tm_column_t* column = &tm->columns[i];
  //   size += sizeof(sl_htm_tm_cell_t) * tm->parameters.num_cells_per_column;
  //   // Go through all the cells
  //   for (uint16_t j = 0; j < tm->parameters.num_cells_per_column; j++) {
  //     sl_htm_tm_cell_t* cell = &column->cells[j];
  //     size += sizeof(sl_htm_tm_segment_t) * cell->num_segments;
  //     // Go through all the segments
  //     for (uint16_t k = 0; k < cell->num_segments; k++) {
  //       sl_htm_tm_segment_t* segment = &cell->segments[k];
  //       size += sizeof(sl_htm_tm_synapse_t) * segment->num_synapses;
  //     }
  //   }
  // }

  // This is the static memory size of the temporal memory
  size += sizeof(sl_htm_tm_t);
  size += sizeof(sl_htm_tm_column_t) * tm->width * tm->height;
  size += sizeof(sl_htm_tm_cell_t) * tm->width * tm->height * tm->parameters.num_cells_per_column;
  size += sizeof(sl_htm_tm_segment_t) * tm->width * tm->height * tm->parameters.num_cells_per_column * tm->parameters.max_segments_in_cell;
  size += sizeof(sl_htm_tm_synapse_t) * tm->width * tm->height * tm->parameters.num_cells_per_column * tm->parameters.max_segments_in_cell * tm->parameters.max_synapses_in_segment;

  size += sl_htm_tm_state_memory_size(&state_prev);
  size += sl_htm_tm_state_memory_size(&state_current);

  return size;
}

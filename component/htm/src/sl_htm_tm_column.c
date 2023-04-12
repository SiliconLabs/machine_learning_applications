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
#include "sl_htm_tm_column.h"
#include "sl_htm_tm_cell.h"
#include "sl_htm_tm_segment.h"
void sl_htm_tm_column_init(sl_htm_tm_t* tm, sl_htm_tm_column_t* column)
{
  column->cells = malloc(sizeof(sl_htm_tm_cell_t) * tm->parameters.num_cells_per_column);
  if (column->cells == NULL) {
    printf("Error [%s:%d]: Could not allocate memory for cells.\n", __FILE__, __LINE__);
    while (1);
  }
  for (uint16_t i = 0; i < tm->parameters.num_cells_per_column; i++) {
    sl_htm_tm_cell_init(tm, &column->cells[i], column);
  }
}

sl_htm_tm_cell_t* sl_htm_tm_column_least_used_cell(sl_htm_tm_t* tm, sl_htm_tm_column_t* column)
{
  sl_htm_tm_cell_t* least_used_cell = &column->cells[0];
  uint16_t min_num_segments = least_used_cell->num_segments;
  for (uint16_t i = 0; i < tm->parameters.num_cells_per_column; i++) {
    sl_htm_tm_cell_t* cell = &column->cells[i];
    // if the number of segments is less than the current minimum, choose this cell
    if (cell->num_segments < min_num_segments) {
      least_used_cell = cell;
      min_num_segments = cell->num_segments;
    } else if (cell->num_segments == min_num_segments) {
      // If the number of segments is the same, choose the cell randomly
      if (rand() % 2 == 0) {
        least_used_cell = cell;
        min_num_segments = cell->num_segments;
      }
    }
  }
  return least_used_cell;
}
/**
 * @brief Find the best matching segment in the column. The best matching segment is the one with the highest potential score.
 *
 */
sl_htm_tm_segment_t* sl_htm_tm_column_best_matching_segment(sl_htm_tm_t* tm, sl_htm_tm_column_t* column, sl_htm_tm_state_t* state_prev)
{
  sl_htm_tm_segment_t* best_segment = NULL;
  int best_potential_score = -1;
  for (uint16_t i = 0; i < state_prev->matching_segments.len; i++) {
    sl_htm_tm_segment_t* segment = state_prev->matching_segments.segments[i];
    if (!sl_htm_tm_segment_is_existing(segment)) {
      continue;
    }
    if (segment->parent_cell->parent_column == column) {
      int potential_score = sl_htm_tm_segment_potential_score(tm, segment, state_prev);
      // if the potential score is greater than the current best, choose this segment
      if (potential_score > best_potential_score) {
        best_segment = segment;
        best_potential_score = potential_score;
      } else if (potential_score == best_potential_score) {
        // If the potential score is the same, choose the segment randomly
        if (rand() % 2 == 0) {
          best_segment = segment;
          best_potential_score = potential_score;
        }
      }
    }
  }
  return best_segment;
}
void sl_htm_tm_column_burst(sl_htm_tm_t* tm, sl_htm_tm_column_t* column, bool learn, sl_htm_tm_state_t* state_current, sl_htm_tm_state_t* state_prev)
{
  // Go through all the cells and add the active ones to the active cells array
  for (uint16_t i = 0; i < tm->parameters.num_cells_per_column; i++) {
    sl_htm_tm_cell_t* cell = &column->cells[i];
    sl_htm_tm_add_cell_to_array(cell, &state_current->active_cells);
  }

  // Count the number of previous matching segments that are on this column
  uint16_t num_matching_segments = 0;
  for (uint16_t i = 0; i < state_prev->matching_segments.len; i++) {
    sl_htm_tm_segment_t* segment = state_prev->matching_segments.segments[i];
    if (sl_htm_tm_segment_is_existing(segment) == false) {
      continue;
    }
    if (segment->parent_cell->parent_column == column) {
      num_matching_segments++;
    }
  }

  sl_htm_tm_cell_t* winner_cell = NULL;
  sl_htm_tm_segment_t* learning_segment = NULL;

  if (num_matching_segments > 0) {
    learning_segment = sl_htm_tm_column_best_matching_segment(tm, column, state_prev);
    if (learning_segment == NULL) {
      printf("Error [%s:%d]: Could not find best matching segment.\n", __FILE__, __LINE__);
      while (1);
    }
    winner_cell = learning_segment->parent_cell;
  } else {
    winner_cell = sl_htm_tm_column_least_used_cell(tm, column);
    if (learn) {
      learning_segment = sl_htm_tm_cell_grow_segment(tm, winner_cell);
      // If no segment could be grown, due to the maximum number of segments being reached, then throw error
      // This should never happen.
      if (learning_segment == NULL) {
        printf("Error [%s:%d]: Could not grow segment.\n", __FILE__, __LINE__);
        while (1);
      }
    }
  }
  sl_htm_tm_add_cell_to_array(winner_cell, &state_current->winner_cells);
  // Learn on cell
  if (learn) {
    sl_htm_tm_segment_update_permanence(tm, learning_segment, state_prev);
    sl_htm_tm_segment_grow_synapses(tm, learning_segment, state_prev);
  }
}

bool sl_htm_tm_column_activate_predicted(sl_htm_tm_t* tm, sl_htm_tm_column_t* activeColumn, bool learn, sl_htm_tm_state_t* state_current, sl_htm_tm_state_t* state_prev)
{
  bool any_predictive_cells = false;
  // Go through all the segments in the previous timestep
  for (uint16_t k = 0; k < state_prev->active_segments.len; k++) {
    sl_htm_tm_segment_t* segment = state_prev->active_segments.segments[k];
    // Check if the segment is on the active column
    if (segment->parent_cell->parent_column == activeColumn) {
      // If the segment is active, then the cell is predictive, and must now be activated since they are on an active column
      any_predictive_cells = true;
      // Add the predictive cell to the active cells and winner cells
      sl_htm_tm_add_cell_to_array(segment->parent_cell, &state_current->active_cells);
      sl_htm_tm_add_cell_to_array(segment->parent_cell, &state_current->winner_cells);
      // And update the permanences of the synapses in the segment
      if (learn) {
        sl_htm_tm_segment_update_permanence(tm, segment, state_prev);
        // Also grow new synapses
        sl_htm_tm_segment_grow_synapses(tm, segment, state_prev);
      }
    }
  }
  if (any_predictive_cells) {
    state_current->num_predictive_and_active_columns++;
  }
  return any_predictive_cells;
}
void sl_htm_tm_column_punish_predicted(sl_htm_tm_t* tm, sl_htm_tm_column_t* column, sl_htm_tm_state_t* state_prev)
{
  // Iterate through all the matching segments in the previous timestep
  for (uint16_t j = 0; j < state_prev->matching_segments.len; j++) {
    sl_htm_tm_segment_t* segment = state_prev->matching_segments.segments[j];
    // The segmant may get deleted in the synapse loop, so we need to make sure we do not segfault here.
    if (!sl_htm_tm_segment_is_existing(segment)) {
      continue;
    }
    if (segment->parent_cell->parent_column == column) {
      // Go through all the synapses in the segment
      for (uint16_t k = 0; k < segment->num_synapses; k++) {
        sl_htm_tm_synapse_t* synapse = &segment->synapses[k];

        if (sl_htm_tm_is_cell_in_array(synapse->target_cell, &state_prev->active_cells)) {
          // Wrap in if statement to avoid decrementing below 0 and causing overflow
          if (synapse->permanence < tm->parameters.synapse_permanence_decrement) {
            sl_htm_tm_segment_delete_synapse(tm, segment, synapse);
          } else {
            synapse->permanence -= tm->parameters.synapse_permanence_decrement;
          }
        }
      }
    }
  }
}

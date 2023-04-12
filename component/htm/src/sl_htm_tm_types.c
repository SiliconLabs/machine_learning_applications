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
#include "sl_htm_tm_types.h"
void sl_htm_tm_init_state(sl_htm_tm_state_t* state)
{
  state->active_cells.len = 0;
  state->active_cells.capacity = 4;
  state->active_cells.cells = NULL;

  state->winner_cells.len = 0;
  state->winner_cells.capacity = 4;
  state->winner_cells.cells = NULL;

  state->active_segments.len = 0;
  state->active_segments.capacity = 4;
  state->active_segments.segments = NULL;

  state->matching_segments.len = 0;
  state->matching_segments.capacity = 4;
  state->matching_segments.segments = NULL;
}
void sl_htm_tm_add_cell_to_array(sl_htm_tm_cell_t* cell, sl_htm_tm_cell_array_t* arr)
{
  //Increment the length
  arr->len++;
  // Grow the array and add the cell
  arr->cells = (sl_htm_tm_cell_t**)realloc(arr->cells, arr->len * sizeof(sl_htm_tm_cell_t*));
  if (arr->cells == NULL) {
    printf("Error [%s:%d]: Could not reallocate memory for cells array.\n", __FILE__, __LINE__);
    while (1);
  }
  arr->cells[arr->len - 1] = cell;
}
bool sl_htm_tm_is_cell_in_array(sl_htm_tm_cell_t* cell, sl_htm_tm_cell_array_t* arr)
{
  for (uint16_t i = 0; i < arr->len; i++) {
    if (arr->cells[i] == cell) {
      return true;
    }
  }
  return false;
}
void sl_htm_tm_shuffle_cell_array(sl_htm_tm_cell_array_t* arr)
{
  for (uint16_t i = 0; i < arr->len; i++) {
    uint16_t j = rand() % arr->len;
    sl_htm_tm_cell_t* tmp = arr->cells[i];
    arr->cells[i] = arr->cells[j];
    arr->cells[j] = tmp;
  }
}

void sl_htm_tm_add_segment_to_array(sl_htm_tm_segment_t* segment, sl_htm_tm_segment_array_t* arr)
{
  //Increment the length
  arr->len++;
  // Grow the array and add the cell
  arr->segments = (sl_htm_tm_segment_t**)realloc(arr->segments, arr->len * sizeof(sl_htm_tm_segment_t*));
  if (arr->segments == NULL) {
    printf("Error [%s:%d]: Could not reallocate memory for segments array.\n", __FILE__, __LINE__);
    while (1);
  }
  arr->segments[arr->len - 1] = segment;
}

void sl_htm_tm_swap_and_clear_cell_array(sl_htm_tm_cell_array_t* prev, sl_htm_tm_cell_array_t* current)
{
  // Swap the arrays inside the struct
  sl_htm_tm_cell_t** temp = prev->cells;

  prev->cells = current->cells;
  prev->len = current->len;

  current->cells = temp;
  // Clear the current array
  current->len = 0;
}

void sl_htm_tm_swap_and_clear_segment_array(sl_htm_tm_segment_array_t* prev, sl_htm_tm_segment_array_t* current)
{
  // Swap the arrays inside the struct
  sl_htm_tm_segment_t** temp = prev->segments;

  prev->segments = current->segments;
  prev->len = current->len;
  current->segments = temp;
  // Clear the current array
  current->len = 0;
}
void sl_htm_tm_swap_and_clear_states(sl_htm_tm_state_t* state_prev, sl_htm_tm_state_t* state_current)
{
  sl_htm_tm_swap_and_clear_cell_array(&state_prev->active_cells, &state_current->active_cells);
  sl_htm_tm_swap_and_clear_cell_array(&state_prev->winner_cells, &state_current->winner_cells);
  sl_htm_tm_swap_and_clear_segment_array(&state_prev->active_segments, &state_current->active_segments);
  sl_htm_tm_swap_and_clear_segment_array(&state_prev->matching_segments, &state_current->matching_segments);

  state_prev->num_predictive_and_active_columns = state_current->num_predictive_and_active_columns;
  state_current->num_predictive_and_active_columns = 0;
}
size_t sl_htm_tm_state_memory_size(sl_htm_tm_state_t* state)
{
  size_t size = 0;
  size += sizeof(sl_htm_tm_cell_t*) * state->active_cells.len;
  size += sizeof(sl_htm_tm_cell_t*) * state->winner_cells.len;
  size += sizeof(sl_htm_tm_segment_t*) * state->active_segments.len;
  size += sizeof(sl_htm_tm_segment_t*) * state->matching_segments.len;

  return size;
}

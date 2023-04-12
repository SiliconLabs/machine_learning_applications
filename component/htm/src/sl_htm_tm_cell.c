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
#include "sl_htm_tm_cell.h"
#include "sl_htm_tm_segment.h"
#include "sl_htm_tm_column.h"
void sl_htm_tm_cell_init(sl_htm_tm_t* tm, sl_htm_tm_cell_t* cell, sl_htm_tm_column_t* parent_column)
{
  cell->segments = malloc(tm->parameters.max_segments_in_cell * sizeof(sl_htm_tm_segment_t));
  if (cell->segments == NULL) {
    printf("Error [%s:%d]: Could not allocate memory for segments.\n", __FILE__, __LINE__);
    while (1);
  }
  for (uint16_t i = 0; i < tm->parameters.max_segments_in_cell; i++) {
    sl_htm_segment_init(tm, &cell->segments[i]);
  }
  cell->parent_column = parent_column;
  cell->num_segments = 0;
}
/**
 * @brief Find the least used segment in the cell. The heuristic used is to sum all the permanences squared of the synapses in the segment.
 *
 */
void sl_htm_tm_cell_delete_least_useful_segment(sl_htm_tm_t* tm, sl_htm_tm_cell_t* cell)
{
  sl_htm_tm_segment_t* least_used_segment = NULL;
  uint8_t max_heuristic_value = 0;
  // Go through all the segments in the cell
  for (uint16_t i = 0; i < cell->num_segments; i++) {
    sl_htm_tm_segment_t* segment = &cell->segments[i];
    if (!sl_htm_tm_segment_is_existing(segment)) {
      continue;
    }
    uint8_t heuristic_value = 0;
    // Go through all the synapses in the segment
    for (uint16_t j = 0; j < segment->num_synapses; j++) {
      sl_htm_tm_synapse_t* synapse = &segment->synapses[j];
      if (!sl_htm_tm_synapse_is_existing(synapse)) {
        continue;
      }
      heuristic_value += synapse->permanence * synapse->permanence;
    }
    if (least_used_segment == NULL || heuristic_value > max_heuristic_value) {
      least_used_segment = segment;
      max_heuristic_value = heuristic_value;
    }
  }
  if (least_used_segment == NULL) {
    printf("Error [%s:%d]: Could not find a segment to delete.\n", __FILE__, __LINE__);
    while (1);
  }
  sl_htm_tm_cell_delete_segment(tm, cell, least_used_segment);
}
sl_htm_tm_segment_t* sl_htm_tm_cell_grow_segment(sl_htm_tm_t* tm, sl_htm_tm_cell_t* cell)
{
  // If all the slots are full, prune the least used segment
  if (cell->num_segments >= tm->parameters.max_segments_in_cell) {
    sl_htm_tm_cell_delete_least_useful_segment(tm, cell);
  }
  // Find an empty segment
  for (uint16_t i = 0; i < tm->parameters.max_segments_in_cell; i++) {
    sl_htm_tm_segment_t* segment = &cell->segments[i];
    if (!sl_htm_tm_segment_is_existing(segment)) {
      segment->parent_cell = cell;
      segment->num_synapses = 0;

      segment->parent_cell->num_segments++;
      return segment;
    }
  }
  return NULL;
}
void sl_htm_tm_cell_delete_segment(sl_htm_tm_t* tm, sl_htm_tm_cell_t* cell, sl_htm_tm_segment_t* segment)
{
  sl_htm_segment_reset(tm, segment);
  cell->num_segments--;
}

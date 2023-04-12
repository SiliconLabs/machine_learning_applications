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
#ifndef SL_HTM_TM_TYPES_H
#define SL_HTM_TM_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_htm_sdr.h"

typedef struct sl_htm_tm_cell sl_htm_tm_cell_t;
typedef struct sl_htm_tm_segment sl_htm_tm_segment_t;
typedef struct sl_htm_tm_column sl_htm_tm_column_t;
/**
 * @brief A synapse is a connection between a segment and a cell.
 *
 */
typedef struct {
  uint8_t permanence;
  sl_htm_tm_cell_t* target_cell; // aka presynaptic cell
  sl_htm_tm_segment_t *parent_segment;
} sl_htm_tm_synapse_t;
/**
 * @brief A segment is a list of synapses.
 *
 */
struct sl_htm_tm_segment{
  sl_htm_tm_synapse_t *synapses;
  uint16_t num_synapses;

  sl_htm_tm_cell_t *parent_cell;
};
/**
 * @brief A cell has a state, and a list of segments.
 *
 */
struct sl_htm_tm_cell{
  sl_htm_tm_segment_t *segments;
  uint16_t num_segments;

  sl_htm_tm_column_t *parent_column;
};
/**
 * @brief A column is a group of cells. It also keeps track of the segments that are connected to the cells in the column.
 *
 */
struct sl_htm_tm_column{
  sl_htm_tm_cell_t *cells;
};

typedef struct {
  uint16_t segment_activation_threshold;
  uint16_t segment_learning_threshold;
  uint8_t synapse_permanence_increment;
  uint8_t synapse_permanence_decrement;
  uint8_t synapse_permanence_initial;
  uint8_t synapse_permanence_threshold;
  uint16_t max_synapses_in_segment;
  uint16_t max_segments_in_cell;
  uint16_t num_cells_per_column;
} sl_htm_tm_parameters_t;
/**
 * @brief A temporal memory is a set of columns, each of which contains a set of cells. The number of cells in a column is the depth of the temporal memory.
 *
 */
typedef struct {
  sl_htm_tm_parameters_t parameters;
  sl_htm_tm_column_t *columns;
  uint16_t width;
  uint16_t height;
  uint16_t input_sparsity;
} sl_htm_tm_t;

typedef struct {
  sl_htm_tm_cell_t** cells;
  uint16_t len;
  uint16_t capacity;
} sl_htm_tm_cell_array_t;

typedef struct {
  sl_htm_tm_segment_t** segments;
  uint16_t len;
  uint16_t capacity;
} sl_htm_tm_segment_array_t;
typedef struct {
  sl_htm_tm_cell_array_t active_cells;
  sl_htm_tm_cell_array_t winner_cells;
  sl_htm_tm_segment_array_t active_segments;
  sl_htm_tm_segment_array_t matching_segments;
  uint16_t num_predictive_and_active_columns;
} sl_htm_tm_state_t;

void sl_htm_tm_init_state(sl_htm_tm_state_t* state);
void sl_htm_tm_add_cell_to_array(sl_htm_tm_cell_t* cell, sl_htm_tm_cell_array_t* arr);
bool sl_htm_tm_is_cell_in_array(sl_htm_tm_cell_t* cell, sl_htm_tm_cell_array_t* arr);

void sl_htm_tm_shuffle_cell_array(sl_htm_tm_cell_array_t* arr);

void sl_htm_tm_add_segment_to_array(sl_htm_tm_segment_t* segment, sl_htm_tm_segment_array_t* arr);

void sl_htm_tm_swap_and_clear_states(sl_htm_tm_state_t* state_prev, sl_htm_tm_state_t* state_current);
size_t sl_htm_tm_state_memory_size(sl_htm_tm_state_t* state);
#ifdef __cplusplus
}
#endif

#endif // SL_HTM_TM_TYPES_H

#include "gtest/gtest.h"
#include "sl_htm_tm.h"
#include "sl_htm_sdr.h"
#include "sl_htm_tm_types.h"
#include "sl_htm_tm_cell.h"
TEST(TMTest, States){
  sl_htm_tm_state_t state_current;
  sl_htm_tm_state_t state_previous;
  sl_htm_tm_init_state(&state_current);
  sl_htm_tm_init_state(&state_previous);
  // Assert that the states are properly initialized
  EXPECT_EQ(state_current.active_cells.len, 0);
  EXPECT_EQ(state_current.winner_cells.len, 0);
  EXPECT_EQ(state_current.active_segments.len, 0);
  EXPECT_EQ(state_current.matching_segments.len, 0);
  // Add a active cell to the current state, use the num_segments field to store an id of the cell used for testing
  sl_htm_tm_cell_t active_cell;
  active_cell.num_segments = 1;

  sl_htm_tm_add_cell_to_array(&active_cell, &state_current.active_cells);

  // Test if cell was added
  EXPECT_EQ(state_current.active_cells.len, 1);
  EXPECT_EQ(state_current.winner_cells.len, 0);
  EXPECT_EQ(state_current.active_segments.len, 0);
  EXPECT_EQ(state_current.matching_segments.len, 0);
  EXPECT_TRUE(sl_htm_tm_is_cell_in_array(&active_cell, &state_current.active_cells));

  // Swap and clear the states, and see if the cell is still there in the previous state
  sl_htm_tm_swap_and_clear_states(&state_previous, &state_current);

  EXPECT_EQ(state_current.active_cells.len, 0);
  EXPECT_EQ(state_current.winner_cells.len, 0);
  EXPECT_EQ(state_current.active_segments.len, 0);
  EXPECT_EQ(state_current.matching_segments.len, 0);

  EXPECT_EQ(state_previous.active_cells.len, 1);
  EXPECT_EQ(state_previous.winner_cells.len, 0);
  EXPECT_EQ(state_previous.active_segments.len, 0);
  EXPECT_EQ(state_previous.matching_segments.len, 0);

  EXPECT_TRUE(sl_htm_tm_is_cell_in_array(&active_cell, &state_previous.active_cells));

  // Add 5 more active cells to the current state
  sl_htm_tm_cell_t active_cell_2;
  active_cell_2.num_segments = 2;
  sl_htm_tm_cell_t active_cell_3;
  active_cell_3.num_segments = 3;
  sl_htm_tm_cell_t active_cell_4;
  active_cell_4.num_segments = 4;
  sl_htm_tm_cell_t active_cell_5;
  active_cell_5.num_segments = 5;
  sl_htm_tm_cell_t active_cell_6;
  active_cell_6.num_segments = 6;

  sl_htm_tm_add_cell_to_array(&active_cell, &state_current.active_cells);
  sl_htm_tm_add_cell_to_array(&active_cell_2, &state_current.active_cells);
  sl_htm_tm_add_cell_to_array(&active_cell_3, &state_current.active_cells);
  sl_htm_tm_add_cell_to_array(&active_cell_4, &state_current.active_cells);
  sl_htm_tm_add_cell_to_array(&active_cell_5, &state_current.active_cells);
  sl_htm_tm_add_cell_to_array(&active_cell_6, &state_current.active_cells);
  // Ensure that the cells are in order
  EXPECT_EQ(state_current.active_cells.cells[0]->num_segments, (uint16_t)1);
  EXPECT_EQ(state_current.active_cells.cells[1]->num_segments, (uint16_t)2);
  EXPECT_EQ(state_current.active_cells.cells[2]->num_segments, (uint16_t)3);
  EXPECT_EQ(state_current.active_cells.cells[3]->num_segments, (uint16_t)4);
  EXPECT_EQ(state_current.active_cells.cells[4]->num_segments, (uint16_t)5);
  EXPECT_EQ(state_current.active_cells.cells[5]->num_segments, (uint16_t)6);

  uint16_t num_out_of_order = 0;
  uint16_t num_in_order = 0;
  // Shuffle multiple times to reduce the chance of the cells being in order
  for (uint16_t i = 0; i < 100; i++) {
    sl_htm_tm_shuffle_cell_array(&state_current.active_cells);
    for (uint16_t i = 0; i < state_current.active_cells.len; i++) {
      if (state_current.active_cells.cells[i]->num_segments != i + 1) {
        num_out_of_order++;
      } else {
        num_in_order++;
      }
    }
  }

  // Ensure that the cells are out of order
  EXPECT_GT(num_out_of_order, 0);
  // But we still expect some of them to be in order by chance
  EXPECT_GT(num_in_order, 0);
}

TEST(TMTest, Learning) {
  // Setup
  sl_htm_sdr_t input_sdr;
  sl_htm_sdr_init(&input_sdr, 20, 20);
  sl_htm_sdr_randomize(&input_sdr, 0.2f);

  sl_htm_tm_t tm;
  sl_htm_tm_init_default_params(&tm.parameters);
  sl_htm_tm_init(&tm, input_sdr.width, input_sdr.height);

  // The first time we execute the TM, we expect the anomaly score to be 1.0
  float anomaly_score = sl_htm_tm_execute(&tm, &input_sdr, true);
  EXPECT_EQ(anomaly_score, 1.0f);

  // After 100 iterations, the anomaly score should be close to 0.0
  for (uint16_t i = 0; i < 100; i++) {
    anomaly_score = sl_htm_tm_execute(&tm, &input_sdr, true);
  }
  EXPECT_LT(anomaly_score, 0.01f);

  // Try with learning disabled
  // The anomaly score should be equal to the last time we executed with learning enabled
  float anomaly_score_2 = sl_htm_tm_execute(&tm, &input_sdr, false);
  EXPECT_EQ(anomaly_score, anomaly_score_2);

  // Randomize the input SDR each iteration
  // With random input each time, the average anomaly score should be close to 1
  float total_anomaly_score = 0.0f;
  uint16_t num_anomaly_scores = 0;
  for (uint16_t i = 0; i < 100; i++) {
    sl_htm_sdr_randomize(&input_sdr, 0.2f);
    total_anomaly_score += sl_htm_tm_execute(&tm, &input_sdr, true);
    num_anomaly_scores++;
  }
  anomaly_score = total_anomaly_score / num_anomaly_scores;
  EXPECT_GT(anomaly_score, 0.9f);

  printf("TM Memory Size: %zu bytes\n", sl_htm_tm_memory_size(&tm));
}

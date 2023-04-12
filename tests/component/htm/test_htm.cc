#include "gtest/gtest.h"
#include "sl_htm.h"

TEST(HTMTest, System){
  sl_htm_sp_parameters_t sp_params;
  sl_htm_tm_parameters_t tm_params;

  sl_htm_sp_init_default_params(&sp_params);
  sp_params.permanence_increment = 25;
  sp_params.permanence_decrement = 25;
  sl_htm_tm_init_default_params(&tm_params);

  sl_htm_init(50, 50, 25, 25, sp_params, tm_params);

  sl_htm_sdr_t input_sdr;
  sl_htm_sdr_init(&input_sdr, 50, 50);
  sl_htm_sdr_randomize(&input_sdr, 0.2f);

  float score = 0;

  for (uint16_t i = 0; i < 200; i++) {
    score = sl_htm_execute(&input_sdr, true);
  }
  // Assert that the score has settled close to zero
  ASSERT_LT(score, 0.1f);
  printf("Executing 100 iterations of 3 simple numbers (0, 1, 2)\n");
  for (uint16_t i = 0; i < 100; i++) {
    for (uint16_t j = 0; j < 3; j++) {
      sl_htm_encoder_simple_number(j, 0, 3, 25, &input_sdr);
      score = sl_htm_execute(&input_sdr, true);
    }
  }
  // Assert that the score has settled close to zero
  ASSERT_LT(score, 0.2f);
  printf("SP memory size: %zu bytes\n", sl_htm_sp_memory_size(sl_htm_get_sp()));
  printf("TM memory size: %zu bytes\n", sl_htm_tm_memory_size(sl_htm_get_tm()));
}

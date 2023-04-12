#include "gtest/gtest.h"
#include "sl_htm_sp.h"

TEST(SPTest, Initialization) {
  sl_htm_sdr_t input_sdr;
  sl_htm_sdr_init(&input_sdr, 100, 100);
  sl_htm_sdr_t output_sdr;
  sl_htm_sdr_init(&output_sdr, 10, 10);

  sl_htm_sp_t sp;
  sl_htm_sp_init_default_params(&sp.parameters);
  sl_htm_sp_init(&sp, input_sdr.width, input_sdr.height, output_sdr.width, output_sdr.height);

  for (int i = 0; i < 100; i++) {
    sl_htm_sdr_randomize(&input_sdr, 0.2f);
    sl_htm_sp_execute(&sp, &input_sdr, &output_sdr, true);
  }
  sl_htm_sp_print(&sp);
  // Assert that the output SDR has the correct number of active bits according to the SP sparsity.
  EXPECT_EQ(output_sdr.num_active_bits, (uint16_t)(sp.parameters.sparsity * output_sdr.width * output_sdr.height));

  // Finally, print the SP memory size
  sp.width = 10;
  sp.height = 10;
  printf("SP memory size: %zu bytes\n", sl_htm_sp_memory_size(&sp));
}

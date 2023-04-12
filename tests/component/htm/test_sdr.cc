#include "gtest/gtest.h"
#include "sl_htm_sdr.h"
#include "sl_htm_utils.h"
#include "sl_htm_encoder.h"
TEST(SDRTest, IndexCoordinates) {
  // Arrange
  sl_htm_sdr_t sdr;
  sdr.width = 2;
  sdr.height = 2;
  sdr.num_active_bits = 0;
  sdr.bits = new bool[4] {
    0, 0,
    1, 0
  };

  // Assert
  EXPECT_EQ(sl_htm_sdr_get_bit(&sdr, sl_htm_utils_xy_to_index(0, 0, sdr.width, sdr.height)), false);
  EXPECT_EQ(sl_htm_sdr_get_bit(&sdr, sl_htm_utils_xy_to_index(0, 1, sdr.width, sdr.height)), true);

  // Convert coordinates to index and back again
  sdr.width = 10;
  sdr.height = 10;

  uint16_t x = 5;
  uint16_t y = 3;

  uint16_t index = sl_htm_utils_xy_to_index(x, y, sdr.width, sdr.height);

  EXPECT_EQ(sl_htm_utils_index_to_x(index, sdr.width), x);
  EXPECT_EQ(sl_htm_utils_index_to_y(index, sdr.width, sdr.height), y);
}
TEST(SDRTest, Randomize) {
  // Arrange
  sl_htm_sdr_t sdr;
  sl_htm_sdr_init(&sdr, 100, 100);
  float target_sparsity = 0.2f;
  sl_htm_sdr_randomize(&sdr, target_sparsity);
  // Assert that the SDR has the correct number of active bits according to the sparsity.
  EXPECT_EQ(sdr.num_active_bits, (uint16_t)(target_sparsity * sdr.width * sdr.height));

  // Randomize again
  target_sparsity = 0.5f;
  sl_htm_sdr_randomize(&sdr, target_sparsity);
  // Assert that the SDR has the correct number of active bits according to the sparsity.
  EXPECT_EQ(sdr.num_active_bits, (uint16_t)(target_sparsity * sdr.width * sdr.height));
}

TEST(SDRTest, Insert){
  // Arrange
  sl_htm_sdr_t target_sdr;
  sl_htm_sdr_init(&target_sdr, 5, 3);

  sl_htm_sdr_t source_sdr1;
  sl_htm_sdr_init(&source_sdr1, 5, 1);
  sl_htm_sdr_set_bit(&source_sdr1, 0, true);

  sl_htm_sdr_t source_sdr2;
  sl_htm_sdr_init(&source_sdr2, 5, 1);
  sl_htm_sdr_set_bit(&source_sdr2, 0, true);

  // Act
  sl_htm_sdr_insert(&target_sdr, &source_sdr1, 0, 0);
  sl_htm_sdr_insert(&target_sdr, &source_sdr2, 0, 1);

  sl_htm_sdr_print(&target_sdr);
  // Assert
  EXPECT_EQ(sl_htm_sdr_get_bit(&target_sdr, 0), true);
  EXPECT_EQ(sl_htm_sdr_get_bit(&target_sdr, 5 * 1), true);
}
TEST(SDRTest, Insert2){
  // Arrange
  sl_htm_sdr_t target_sdr;
  sl_htm_sdr_init(&target_sdr, 30, 30);

  sl_htm_sdr_t source_sdr1;
  sl_htm_sdr_init(&source_sdr1, 30, 10);
  sl_htm_encoder_simple_number(-5, -10, 10, 10, &source_sdr1);

  sl_htm_sdr_t source_sdr2;
  sl_htm_sdr_init(&source_sdr2, 30, 10);
  sl_htm_encoder_simple_number(5, 0, 10, 10, &source_sdr2);

  // Act
  sl_htm_sdr_insert(&target_sdr, &source_sdr1, 0, 0);
  sl_htm_sdr_insert(&target_sdr, &source_sdr2, 0, 10);

  sl_htm_sdr_print(&target_sdr);
  // Assert
  EXPECT_EQ(target_sdr.num_active_bits, 20);
}

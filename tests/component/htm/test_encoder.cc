#include "gtest/gtest.h"
#include "sl_htm_encoder.h"
TEST(EncoderTest, SimpleNumber) {
  sl_htm_sdr_t sdr;
  sl_htm_sdr_init(&sdr, 10, 10);

  sl_htm_encoder_simple_number(1, 1, 10, 10, &sdr);

  // Assert that the first 7 bits are set.
  for (int i = 0; i < 7; i++) {
    EXPECT_EQ(1, sdr.bits[i]);
  }
  sl_htm_sdr_t sdr2;
  sl_htm_sdr_init(&sdr2, 10, 10);
  sl_htm_encoder_simple_number(2, 1, 10, 20, &sdr2);

  sl_htm_sdr_print(&sdr2);
}

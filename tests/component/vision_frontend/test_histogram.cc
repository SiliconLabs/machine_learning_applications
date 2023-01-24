#include "gtest/gtest.h"
#include "image_utils.h"
#include "histogram.h"

TEST(FrontendTest, CalculateCDF) {
  const size_t hist[5] = { 1, 2, 3, 4, 5 };
  size_t cdf[5];
  size_t hist_levels = 5;

  calculate_cdf(hist, cdf, hist_levels);

  EXPECT_EQ(cdf[0], 1);
  EXPECT_EQ(cdf[1], 3);
  EXPECT_EQ(cdf[2], 6);
  EXPECT_EQ(cdf[3], 10);
  EXPECT_EQ(cdf[4], 15);
}
TEST(FrontendTest, HistogramEqualize) {
  //Arrange
  struct Image src_img;
  generate_empty_image(&src_img, 3, 3, 1, IMAGEFORMAT_UINT8);
  src_img.data.i[0] = 0;
  src_img.data.i[1] = 1;
  src_img.data.i[2] = 2;
  src_img.data.i[3] = 3;

  struct Image dst_img;
  generate_empty_image(&dst_img, 3, 3, 1, IMAGEFORMAT_UINT8);

  //Act
  histogram_equalize(&src_img, &dst_img);

  //Assert
  EXPECT_EQ(dst_img.data.i[0], 0);
  EXPECT_EQ(dst_img.data.i[1], 85);
  EXPECT_EQ(dst_img.data.i[2], 170);
  EXPECT_EQ(dst_img.data.i[3], 255);
}

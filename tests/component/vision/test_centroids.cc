#include "gtest/gtest.h"
#include "sl_vision_image.h"
#include "sl_vision_centroid.h"

TEST(FrontendTest, FindCentroidsConnectedPixels) {
  // Arrange
  sl_vision_image_t src_img;
  src_img.width = 4;
  src_img.height = 4;
  src_img.depth = 1;
  src_img.format = IMAGEFORMAT_FLOAT;
  src_img.data.f = new float[4 * 4] { 2, 2, 2, 2,
                                      0, 0, 0, 0,
                                      0, 2, 2, 0,
                                      0, 2, 2, 0 };

  sl_vision_image_t label_img;
  sl_vision_image_generate_empty(&label_img, src_img.width, src_img.height, src_img.depth, IMAGEFORMAT_UINT8);

  float threshold = 1.4f;
  uint8_t num_labels = sl_vision_image_connected_pixels(&label_img, &src_img, threshold);

  sl_vision_image_print(&label_img);
  sl_vision_centroid_t centroids_out[num_labels];

  // Act
  sl_vision_centroid_from_connected_pixels(&label_img, &src_img, centroids_out, num_labels);

  // Assert
  EXPECT_FLOAT_EQ(centroids_out[0].x, 2.0f);
  EXPECT_FLOAT_EQ(centroids_out[0].y, 0.5f);
  EXPECT_EQ(centroids_out[0].count, 4);

  EXPECT_FLOAT_EQ(centroids_out[1].x, 2.0f);
  EXPECT_FLOAT_EQ(centroids_out[1].y, 3.0f);
  EXPECT_EQ(centroids_out[1].count, 4);
}

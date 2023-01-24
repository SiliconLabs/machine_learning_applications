#include "gtest/gtest.h"
#include "image_utils.h"
#include "centroids.h"

TEST(FrontEndTest, FindCentroidsConnectedPixels) {
  // Arrange
  struct Image src_img;
  src_img.width = 4;
  src_img.height = 4;
  src_img.depth = 1;
  src_img.format = IMAGEFORMAT_FLOAT;
  src_img.data.f = new float[4 * 4] { 2, 2, 2, 2,
                                      0, 0, 0, 0,
                                      0, 2, 2, 0,
                                      0, 2, 2, 0 };

  struct Image label_img;
  generate_empty_image(&label_img, src_img.width, src_img.height, src_img.depth, IMAGEFORMAT_UINT8);
  uint32_t working_memory_len = src_img.width * src_img.height * src_img.depth;
  struct queue_pixel_entry working_memory[working_memory_len];
  float threshold = 1.4f;
  uint8_t num_labels = find_connected_pixels(&label_img, &src_img, threshold, working_memory, working_memory_len);

  print_image(&label_img);
  struct centroid centroids_out[num_labels];

  // Act
  find_centroids_connected_pixels(&label_img, &src_img, centroids_out, num_labels);

  // Assert
  EXPECT_FLOAT_EQ(centroids_out[0].x, 2.0f);
  EXPECT_FLOAT_EQ(centroids_out[0].y, 0.5f);
  EXPECT_EQ(centroids_out[0].count, 4);

  EXPECT_FLOAT_EQ(centroids_out[1].x, 2.0f);
  EXPECT_FLOAT_EQ(centroids_out[1].y, 3.0f);
  EXPECT_EQ(centroids_out[1].count, 4);
}

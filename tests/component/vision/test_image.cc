#include "gtest/gtest.h"
#include "sl_vision.h"
#include "sl_vision_image.hpp"

TEST(FrontendTest, Clamp) {
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
  EXPECT_EQ(sl_vision_clampf(10, 0, 5), 5);
}
TEST(FrontendTest, CenterCrop_uint8) {
  // Arrange
  sl_vision_image_t src_img;
  sl_vision_image_generate_random(&src_img, 200, 200, 3, IMAGEFORMAT_UINT8);

  sl_vision_image_t dst_img;
  sl_vision_image_generate_random(&dst_img, 200, 200, 3, IMAGEFORMAT_UINT8);

  size_t start_x = (src_img.width - dst_img.width) / 2;
  size_t start_y = (src_img.height - dst_img.height) / 2;
  size_t end_x = start_x + dst_img.width;
  size_t end_y = start_y + dst_img.height;

  // Act
  sl_vision_image_crop_center(&src_img, &dst_img);

  // Assert
  for (size_t x = start_x; x < end_x; x++) {
    for (size_t y = start_y; y < end_y; y++) {
      for (size_t z = 0; z < src_img.depth; z++) {
        uint8_t src_val = generic_sl_vision_image_pixel_get_value<uint8_t>(&src_img, x, y, z);
        uint8_t dst_val = generic_sl_vision_image_pixel_get_value<uint8_t>(&dst_img, x - start_x, y - start_y, z);
        EXPECT_EQ(src_val, dst_val);
      }
    }
  }
}
TEST(FrontendTest, CenterCrop_float) {
  // Arrange
  sl_vision_image_t src_img;
  sl_vision_image_generate_random(&src_img, 200, 200, 3, IMAGEFORMAT_FLOAT);

  sl_vision_image_t dst_img;
  sl_vision_image_generate_random(&dst_img, 100, 100, 3, IMAGEFORMAT_FLOAT);

  size_t start_x = (src_img.width - dst_img.width) / 2;
  size_t start_y = (src_img.height - dst_img.height) / 2;
  size_t end_x = start_x + dst_img.width;
  size_t end_y = start_y + dst_img.height;

  // Act
  sl_vision_image_crop_center(&src_img, &dst_img);

  // Assert
  for (size_t x = start_x; x < end_x; x++) {
    for (size_t y = start_y; y < end_y; y++) {
      for (size_t z = 0; z < src_img.depth; z++) {
        float src_val = generic_sl_vision_image_pixel_get_value<float>(&src_img, x, y, z);
        float dst_val = generic_sl_vision_image_pixel_get_value<float>(&dst_img, x - start_x, y - start_y, z);
        EXPECT_EQ(src_val, dst_val);
      }
    }
  }
}
TEST(FrontendTest, ConnectedPixels_float){
  //Arrange
  sl_vision_image_t src_img;
  src_img.width = 3;
  src_img.height = 3;
  src_img.depth = 1;
  src_img.format = IMAGEFORMAT_FLOAT;
  float src_img_data[9] = { 0, 0, 0, 0.9f, 0.9f, 0.9f, 0, 0, 0 };
  src_img.data.f = src_img_data;

  sl_vision_image_t dst_label_img;
  dst_label_img.width = 3;
  dst_label_img.height = 3;
  dst_label_img.depth = 1;
  dst_label_img.format = IMAGEFORMAT_UINT8;
  uint8_t dst_label_img_data[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  dst_label_img.data.i = dst_label_img_data;

  //Act

  float threshold = 0.5f;
  uint8_t num_labels = sl_vision_image_connected_pixels(&dst_label_img, &src_img, threshold);

  //Assert
  EXPECT_EQ(num_labels, 1);
  EXPECT_EQ(dst_label_img_data[4], 1);
  EXPECT_EQ(dst_label_img_data[5], 1);
  EXPECT_EQ(dst_label_img_data[6], 0);
}
TEST(FrontendTest, ConnectedPixels_uint8){
  //Arrange
  sl_vision_image_t src_img;
  src_img.width = 3;
  src_img.height = 3;
  src_img.depth = 1;
  src_img.format = IMAGEFORMAT_UINT8;
  uint8_t src_img_data[9] = { 0, 0, 0, 2, 2, 2, 0, 0, 0 };
  src_img.data.i = src_img_data;

  sl_vision_image_t dst_label_img;
  dst_label_img.width = 3;
  dst_label_img.height = 3;
  dst_label_img.depth = 1;
  dst_label_img.format = IMAGEFORMAT_UINT8;
  uint8_t dst_label_img_data[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  dst_label_img.data.i = dst_label_img_data;

  //Act
  float threshold = 1.4f;
  uint8_t num_labels = sl_vision_image_connected_pixels(&dst_label_img, &src_img, threshold);
  for (int i = 0; i < 9; i++) {
    printf("%d ", dst_label_img_data[i]);
  }
  //Assert
  EXPECT_EQ(num_labels, 1);
  EXPECT_EQ(dst_label_img_data[4], 1);
  EXPECT_EQ(dst_label_img_data[5], 1);
  EXPECT_EQ(dst_label_img_data[6], 0);
}

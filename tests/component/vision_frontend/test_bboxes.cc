#include "gtest/gtest.h"
#include "image_utils.h"
#include "bboxes.h"

TEST(FrontendTest, CalculateIoU) {
  struct bbox a = { 0, 0, 10, 10 };
  struct bbox b = { 5, 5, 5, 5 };
  struct bbox c = { 0, 0, 10, 10 };
  struct bbox d = { 10, 10, 20, 20 };
  float iou1 = calculate_iou(&a, &b);
  float iou2 = calculate_iou(&c, &d);

  EXPECT_FLOAT_EQ(iou1, 0.25);
  EXPECT_FLOAT_EQ(iou2, 0.0);
}

TEST(FrontendTest, NonMaxSuppression) {
  // Arrange
  struct bbox bboxes[5] = {
    { 0, 0, 5, 5, 0, 1.0 },
    { 0, 0, 10, 10, 0, 0.5 },
    { 20, 20, 30, 30, 0, 1.0 },
    { 15, 15, 25, 25, 0, 0.75 },
    { 30, 30, 40, 40, 0, 1.0 }
  };
  size_t max_num_bboxes = 5;
  struct bbox bboxes_out[5];
  float iou_threshold = 0.1f;
  // Act
  size_t num_remaining = non_max_suppression(bboxes, max_num_bboxes, bboxes_out, iou_threshold);

  // Assert
  EXPECT_EQ(num_remaining, 3);

  EXPECT_EQ(bboxes_out[0].x, 30);
  EXPECT_EQ(bboxes_out[0].y, 30);
  EXPECT_EQ(bboxes_out[0].width, 40);
  EXPECT_EQ(bboxes_out[0].height, 40);
  EXPECT_FLOAT_EQ(bboxes_out[0].confidence, 1.0);

  EXPECT_EQ(bboxes_out[1].x, 0);
  EXPECT_EQ(bboxes_out[1].y, 0);
  EXPECT_EQ(bboxes_out[1].width, 5);
  EXPECT_EQ(bboxes_out[1].height, 5);
  EXPECT_FLOAT_EQ(bboxes_out[1].confidence, 1.0);

  EXPECT_EQ(bboxes_out[2].x, 15);
  EXPECT_EQ(bboxes_out[2].y, 15);
  EXPECT_EQ(bboxes_out[2].width, 25);
  EXPECT_EQ(bboxes_out[2].height, 25);
  EXPECT_FLOAT_EQ(bboxes_out[2].confidence, 0.75);
}

#include "gtest/gtest.h"
#include "sl_vision_image.h"
#include "sl_vision_bbox.h"

TEST(FrontendTest, CalculateIoU) {
  sl_vision_bbox_t a = { 0, 0, 10, 10 };
  sl_vision_bbox_t b = { 5, 5, 5, 5 };
  sl_vision_bbox_t c = { 0, 0, 10, 10 };
  sl_vision_bbox_t d = { 10, 10, 20, 20 };
  float iou1 = sl_vision_bbox_calculate_iou(&a, &b);
  float iou2 = sl_vision_bbox_calculate_iou(&c, &d);

  EXPECT_FLOAT_EQ(iou1, 0.25);
  EXPECT_FLOAT_EQ(iou2, 0.0);
}

TEST(FrontendTest, NonMaxSuppression) {
  // Arrange abcdefgdghh
  sl_vision_bbox_t bboxes[5] = {
    { 0, 0, 5, 5, 0, 1.0 },
    { 0, 0, 10, 10, 0, 0.5 },
    { 20, 20, 30, 30, 0, 1.0 },
    { 15, 15, 25, 25, 0, 0.75 },
    { 30, 30, 40, 40, 0, 1.0 }
  };
  size_t max_num_bboxes = 5;
  sl_vision_bbox_t bboxes_out[5];
  float iou_threshold = 0.1f;
  // Act
  size_t num_remaining = sl_vision_bbox_non_max_suppression(bboxes, max_num_bboxes, bboxes_out, iou_threshold);

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

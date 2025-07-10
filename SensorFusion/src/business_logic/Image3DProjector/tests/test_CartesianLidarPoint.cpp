
#include <gtest/gtest.h>
#include "../CartesianLidarPoint.h"
#include <cmath>

constexpr float EPSILON = 1e-4f;

TEST(CartesianLidarPointTest, OriginPoint) {
    CartesianLidarPoint point(0.0f, 0.0f, 0.0f);
    EXPECT_NEAR(point.xCoord, 0.0f, EPSILON);
    EXPECT_NEAR(point.yCoord, 0.0f, EPSILON);
    EXPECT_NEAR(point.zCoord, 0.0f, EPSILON);
}

TEST(CartesianLidarPointTest, HorizontalForward) {
    CartesianLidarPoint point(1.0f, 0.0f, 0.0f);
    EXPECT_NEAR(point.xCoord, 1.0f, EPSILON);
    EXPECT_NEAR(point.yCoord, 0.0f, EPSILON);
    EXPECT_NEAR(point.zCoord, 0.0f, EPSILON);
}

TEST(CartesianLidarPointTest, HorizontalLeft) {
    CartesianLidarPoint point(1.0f, 90.0f, 0.0f);
    EXPECT_NEAR(point.xCoord, 0.0f, EPSILON);
    EXPECT_NEAR(point.yCoord, 1.0f, EPSILON);
    EXPECT_NEAR(point.zCoord, 0.0f, EPSILON);
}

TEST(CartesianLidarPointTest, VerticalUp) {
    CartesianLidarPoint point(1.0f, 0.0f, 90.0f);
    EXPECT_NEAR(point.xCoord, 0.0f, EPSILON);
    EXPECT_NEAR(point.yCoord, 0.0f, EPSILON);
    EXPECT_NEAR(point.zCoord, 1.0f, EPSILON);
}

TEST(CartesianLidarPointTest, EqualityOperator) {
    CartesianLidarPoint p1(1.0f, 45.0f, 0.0f);
    CartesianLidarPoint p2(1.0f, 45.0f, 0.0f);
    EXPECT_TRUE(p1 == p2);
}

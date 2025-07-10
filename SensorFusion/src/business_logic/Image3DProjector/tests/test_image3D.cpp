#include <gtest/gtest.h>
#include "../Image3DProjector.h"

TEST(Image3DProjectorTest, MultiplyMatrixBasic) {
    Image3DProjector projector;

    std::vector<std::vector<float>> a = {
        {1, 2},
        {3, 4}
    };

    std::vector<std::vector<float>> b = {
        {5, 6},
        {7, 8}
    };

    std::vector<std::vector<float>> expected = {
        {19, 22},
        {43, 50}
    };

    auto result = projector.multiplyMatrix(a, b);

    ASSERT_EQ(result.size(), expected.size());
    ASSERT_EQ(result[0].size(), expected[0].size());

    for (size_t i = 0; i < result.size(); ++i) {
        for (size_t j = 0; j < result[0].size(); ++j) {
            EXPECT_NEAR(result[i][j], expected[i][j], 1e-4f);
        }
    }
}

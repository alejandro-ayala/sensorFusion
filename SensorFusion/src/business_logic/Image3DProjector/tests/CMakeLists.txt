cmake_minimum_required(VERSION 3.10)
project(UnitTests)

set(CMAKE_CXX_STANDARD 11)

# Add GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Add source files
add_executable(
  test_cartesian
  ../CartesianLidarPoint.cpp
  test_CartesianLidarPoint.cpp
)

add_executable(
  test_image3d
  ../Image3DProjector.cpp
  test_Image3DProjector.cpp
)

# Link with GoogleTest
target_link_libraries(test_cartesian gtest_main pthread)
target_link_libraries(test_image3d gtest_main pthread)

include(GoogleTest)
gtest_discover_tests(test_cartesian)
gtest_discover_tests(test_image3d)
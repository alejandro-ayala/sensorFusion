g++ -std=c++11 test_CartesianLidarPoint.cpp ../CartesianLidarPoint.cpp -lgtest -lgtest_main -pthread -o test_cartesian


g++ -std=c++11 test_image3D.cpp ../Image3DProjector.cpp ../Coordinate3DPoint.cpp ../CartesianLidarPoint.cpp -lgtest -lgtest_main -pthread -o test_image3d


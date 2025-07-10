sudo apt update
sudo apt install libgtest-dev
sudo apt install cmake g++ # si no los tienes

# Compilar Google Test (no se instala automáticamente)
cd /usr/src/gtest
sudo cmake .
sudo make
sudo cp lib/*.a /usr/lib

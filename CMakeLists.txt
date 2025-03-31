cmake_minimum_required(VERSION 3.10.0)
project(gcxtool VERSION 0.1.0 LANGUAGES C CXX)

add_executable(gcxtool bin.cpp Blowfish.cpp crypto.cpp decryptor.cpp gcx.cpp main.cpp md5.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


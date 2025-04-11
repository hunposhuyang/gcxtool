cmake_minimum_required(VERSION 3.10.0)
project(gcxtool VERSION 0.1.0 LANGUAGES C CXX)

add_executable(gcxtool main.cpp gcx.cpp crypto.cpp md5.cpp blowfish.cpp decryptor.cpp) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#设置可执行文件的名称和路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

#设置静态链接
# 设置编译器选项为静态链接
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
# 强制使用静态版本的 C++ 标准库
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")

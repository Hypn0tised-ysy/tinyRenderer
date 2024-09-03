cmake_minimum_required(VERSION 3.10)

# 项目名称
project(triangleRasterize)

# 指定 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加源文件
add_executable(TriangleRenderer main.cpp triangle.cpp tgaimage.cpp model.cpp geometry.cpp)





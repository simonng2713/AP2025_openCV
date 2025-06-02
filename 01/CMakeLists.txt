# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(MyOpenCVProject CXX)

add_definitions(-std=c++11)


# ソールファイルをここで指定．ファイル分割したときも，ここに追記．
set(SOURCES
    opencvTest_first.cpp
)

# Create Main project
add_executable(main
    ${SOURCES}
)

# For OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(main ${OpenCV_LIBS})



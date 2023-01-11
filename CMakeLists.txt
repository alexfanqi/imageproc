cmake_minimum_required(VERSION 3.18)

project(image VERSION 0.1.0 LANGUAGES CXX)

add_library(image src/image.cpp)

target_include_directories(image PUBLIC include)

target_compile_features(image PUBLIC cxx_std_20)

option(IMAGE_BENCHMARK "Build benchmark" OFF)

find_package(fmt REQUIRED)
find_package(OpenCV REQUIRED)

target_include_directories(image PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(image PUBLIC
    fmt::fmt
    ${OpenCV_LIBS}
    # Qt5::Core Qt5::Widgets
)

add_executable(image_main src/main.cpp)

target_link_libraries(image_main PRIVATE image)

if (IMAGE_BENCHMARK)
    add_executable(image_bench src/bench.cpp)

    find_package(benchmark REQUIRED)
    target_link_libraries(image_bench PRIVATE image benchmark::benchmark benchmark::benchmark_main)
endif()


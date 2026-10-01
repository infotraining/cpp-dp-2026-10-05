cmake_minimum_required(VERSION 3.30)

project(ShapesJson LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(Dependencies)

add_library(ShapesLib STATIC
    point.cpp
    shape_json_parser.hpp
    circle.hpp
    rectangle.hpp
    square.hpp
    shape_group.hpp
)

target_include_directories(ShapesLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ShapesLib
    PUBLIC
        nlohmann_json::nlohmann_json
)

enable_testing()
add_subdirectory(tests)
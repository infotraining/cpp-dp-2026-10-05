#----------------------------------------
# VCPKG integration
#----------------------------------------
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

if (DEFINED CMAKE_TOOLCHAIN_FILE)
  message(STATUS "Vcpkg integration script found: " ${CMAKE_TOOLCHAIN_FILE})
else()
  message(STATUS "Vcpkg integration script NOT found!")
endif()

#----------------------------------------
# Project
#----------------------------------------
get_filename_component(PROJECT_NAME_STR ${CMAKE_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${PROJECT_NAME_STR})
SET(PROJECT_NAME ${PROJECT_NAME_STR}_tests)

cmake_minimum_required(VERSION 3.24)
project(${PROJECT_NAME} CXX)

#----------------------------------------
# Libraries
#----------------------------------------
find_package(Threads REQUIRED)

find_package(Catch2 3 REQUIRED)
include(Catch)

# find_package(Boost)
# target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost)

# find_package(folly CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE Folly::folly Folly::folly_deps Folly::follybenchmark Folly::folly_test_util)


#----------------------------------------
# Source/Headers/Executable
#----------------------------------------
aux_source_directory(. SRC_LIST)
file(GLOB HEADERS_LIST "*.h" "*.hpp")

enable_testing()

add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADERS_LIST})

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
catch_discover_tests(${PROJECT_NAME})

#----------------------------------------
# Compile options
#----------------------------------------
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)


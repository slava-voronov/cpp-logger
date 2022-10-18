cmake_minimum_required(VERSION 3.14)

project(cpp-logger VERSION "1.0" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(logger)
add_subdirectory(app)

add_subdirectory(tests)
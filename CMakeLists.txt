cmake_minimum_required(VERSION 3.11)

project(hwt LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)
find_package(SFML 2.5 REQUIRED graphics window system)
find_package(CLI11 REQUIRED)
find_package(SQLite3 REQUIRED)

enable_testing()

add_executable(hwt src/main.cpp)
target_link_libraries(hwt PRIVATE SQLite::SQLite3)
target_compile_features(hwt PUBLIC cxx_std_20)
target_include_directories(hwt PRIVATE ./includes)
target_link_libraries(hwt PRIVATE CLI11::CLI11)
target_link_libraries(hwt PRIVATE sfml-graphics sfml-window sfml-system)

add_subdirectory(test/unit_tests)
add_subdirectory(benchmark)

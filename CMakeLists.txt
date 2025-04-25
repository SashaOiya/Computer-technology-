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

foreach(TREE_PROJECT set optim_hwt)
    add_executable(${TREE_PROJECT} src/driver.cpp)
    target_link_libraries(${TREE_PROJECT} PRIVATE SQLite::SQLite3)
    target_compile_features(${TREE_PROJECT} PUBLIC cxx_std_20)
    target_include_directories(${TREE_PROJECT} PRIVATE ./includes)
    target_link_libraries(${TREE_PROJECT} PRIVATE CLI11::CLI11)
    target_link_libraries(${TREE_PROJECT} PRIVATE sfml-graphics sfml-window sfml-system)
endforeach()

add_subdirectory(test/e2e_tests)
add_subdirectory(test/unit_tests)
add_subdirectory(benchmark)

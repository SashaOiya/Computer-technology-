message(STATUS "Conan: Using CMakeDeps conandeps_legacy.cmake aggregator via include()")
message(STATUS "Conan: It is recommended to use explicit find_package() per dependency instead")

find_package(fmt)
find_package(CLI11)
find_package(benchmark)
find_package(GTest)

set(CONANDEPS_LEGACY  fmt::fmt  CLI11::CLI11  benchmark::benchmark_main  gtest::gtest )
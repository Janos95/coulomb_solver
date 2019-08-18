cmake_minimum_required(VERSION 3.14)
project(coulomb_solver)

find_package(Threads REQUIRED)
find_package(OpenMP)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 17)

add_executable(cs
        src/main.cpp
        src/coulomb_solver.hpp
        src/file_io.hpp
        src/struct_of_arrays.hpp src/tuple_helper.hpp)

target_link_libraries(cs PRIVATE
        CONAN_PKG::fmt
        CONAN_PKG::openmpi
        Threads::Threads
        OpenMP::OpenMP_CXX)
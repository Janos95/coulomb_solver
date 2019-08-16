cmake_minimum_required(VERSION 3.14)
project(coulomb_solver)

find_package(Threads REQUIRED)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 17)

add_executable(cs
        src/main.cpp
        src/coulomb_solver.cpp
        src/coulomb_solver.hpp
        src/file_io.cpp
        src/file_io.hpp
        src/binary_operations.hpp
        src/unroll.hpp
        src/vector_traits.hpp
        src/vector.hpp)

target_link_libraries(cs PRIVATE
        CONAN_PKG::fmt
        Threads::Threads)
cmake_minimum_required(VERSION 3.14)
project(coulomb_solver)

find_package(Threads REQUIRED)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 17)

add_executable(cs
        main.cpp
        file_io.cpp
        file_io.hpp
        binary_operations.hpp
        unroll.hpp
        vector_traits.hpp
        vector.hpp)

target_link_libraries(cs PRIVATE
        CONAN_PKG::fmt
        Threads::Threads)
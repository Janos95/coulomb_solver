//
// Created by janos on 18.08.19.
//


#include "file_io.hpp"
#include "struct_of_arrays.hpp"
#include "coulomb_solver.hpp"

#include <fmt/core.h>

#include <string>
#include <chrono>


using RealT = double;

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    std::string filename{"/home/janos/coulomb_solver/assets/test.txt"};

    auto start = std::chrono::system_clock::now();

    auto points = std::make_shared<Points4<RealT>>(readAndParseFile<RealT>(filename));
    fmt::print("Loaded {} points from file\n", points->size());

    CoulombSolver<RealT, Options::ComputeEnergy> solver(points);
    solver.computeAccelerations();

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;

    fmt::print("On time step for {} pts took {}s to complete\n", points->size(), elapsed_seconds.count());

    MPI_Finalize();
}










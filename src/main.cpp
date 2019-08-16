//
// Created by janos on 8/5/19.
//



#include "file_io.hpp"

#include <fmt/core.h>

#include <string>

#include <chrono>
#include <fstream>

int generateTestFile (const int length, const std::string& filename)
{
    std::ofstream file(filename);
    for (int i = 0; i < length; ++i)
    {
        file << fmt::format("{} {} {} {}\n", rand()/RAND_MAX, rand()/RAND_MAX, rand()/RAND_MAX, rand()/RAND_MAX);
    }
}


int main()
{

    std::string filename{"/home/janos/coulomb_solver/assets/test.txt"};
    auto start = std::chrono::system_clock::now();
    auto points = loadFile(filename);
    fmt::print("Loaded {} points from file\n", points.size());
    //CoulombSolver solver;
    //solver.setCharges(&points);
    //auto solution = solver.solve<1>();
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;

    fmt::print("On time step for {} pts took {}s to complete\n", points.size(), elapsed_seconds.count());
}










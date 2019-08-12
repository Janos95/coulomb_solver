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


class CoulombSolver
{
public:
    const std::vector<Vector4f>* setCharges(const std::vector<Vector4f> * charges) { m_charges = charges;}

    template<bool ComputerPotentials>
    [[nodiscard]] auto solve()
    {
        auto& c = *m_charges;
        auto& f = m_coulombForces;
        int numCharges = c.size();

        f.clear();
        f.resize(numCharges, Vector4f(0,0,0,0));

#pragma omp parallel for collapse(2)
        for (std::size_t i = 0; i < numCharges; ++i)
        {
            for(std::size_t j = 0; j < numCharges; ++j)
            {
                // entry (i,j) represents force
                auto force = c[i] - c[j];
                auto norm = force.norm();

                float charge = f[i][3];

                f[i] += 1/powf(norm, 3) * force;

                if constexpr (ComputerPotentials)
                {
                    f[i][3] = charge + c[j][3]/norm;
                }

            }
        }

        return &f;
    }

private:

    const std::vector<Vector4f> * m_charges;
    std::vector<Vector4f> m_coulombForces;
};

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










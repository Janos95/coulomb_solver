//
// Created by janos on 8/13/19.
//

#include "coulomb_solver.hpp"


void CoulombSolver::setCharges(const std::vector<Vector4f> * charges) { m_charges = charges;}

template<bool ComputePotentials>
[[nodiscard]] std::vector<Vector4f> CoulombSolver::solve()
{
    auto& c = *m_charges;
    int numCharges = c.size();

    std::vector<Vector4f> f(numCharges, Vector4f(0.f,0.f,0.f,0.f));

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

            if constexpr (ComputePotentials)
            {
                f[i][3] = charge + c[j][3]/norm;
            }

        }
    }

    return f;
}
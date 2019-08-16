//
// Created by janos on 8/13/19.
//

#pragma once

#include "vector.hpp"

#include <vector>


/** @brief Class to perform a single time step to solve the coulomb equation

    Detailed description follows here.
    @author Janos Meny
    @date August 2019
    */
class CoulombSolver
{
public:
    /** Setter function for particles. The first three
        components of each Vector3f describe the x, y and
        z component, the 4'th component stores the charge q.
        @param charges - particles and charges
        @return void
        */
    void setCharges(const std::vector<Vector4f> * charges);

    /** Solves one time step of the coulomb equation.
        @param bool - ComputePotentials, if this is true, in the
        the forth component will contain also the force potentials.
        @return a vector of Vector4f
        */
    template<bool ComputePotentials>
    [[nodiscard]] std::vector<Vector4f> solve();

private:

    const std::vector<Vector4f> * m_charges;
};

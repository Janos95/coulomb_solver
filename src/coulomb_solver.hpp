//
// Created by janos on 18.08.19.
//

#pragma once

#include "struct_of_arrays.hpp"

#include <vector>
#include <unordered_map>
#include <memory>
#include <thread>
#include <cmath>


template<typename T>
constexpr MPI_Datatype typeToMPI()
{
    static_assert(std::is_floating_point_v<T>);
    if constexpr(std::is_same_v<T, double>)
        return MPI_DOUBLE;
    if constexpr(std::is_same_v<T, float>)
        return MPI_FLOAT;
    if constexpr(std::is_same_v<T, long double>)
        return MPI_LONG_DOUBLE;
}


template<typename T, int options>
void compute_accelerations(
        const T * __restrict__ xs,
        const T * __restrict__ ys,
        const T * __restrict__ zs,
        const T * __restrict__ qs,
        T * __restrict__ accxs,
        T * __restrict__ accys,
        T * __restrict__ acczs,
        T * __restrict__ potentials,
        T& energy,
        int n, int begin, int end,
        int numThreads
        )
{

    std::fill_n(accxs, n, T{0});
    std::fill_n(accys, n, T{0});
    std::fill_n(acczs, n, T{0});

    if constexpr(options == 1 || options == 3)
    {
        std::fill_n(potentials + begin, end - begin, T{0});
    }

    if constexpr(options == 2 || options == 3)
    {
        energy = 0;
    }

#pragma omp parallel for reduction(+:energy)
    for (int i = begin; i < end; ++i)
    {
        T potential{0};

        for(int j = 0; j < n; ++j)
        {
            T accx = xs[i] - xs[j];
            T accy = ys[i] - ys[j];
            T accz = zs[i] - zs[j];

            T inverseSqrt = T{1} / std::sqrt(accx * accx + accy * accy + accz * accz);

            if constexpr (options != 0)
            {
                fmt::print("Computing potentials\n");
                potential += qs[j] * inverseSqrt;
            }


            accxs[i] += accx * std::pow(inverseSqrt, 3) * qs[j];
            accys[i] += accy * std::pow(inverseSqrt, 3) * qs[j];
            acczs[i] += accz * std::pow(inverseSqrt, 3) * qs[j];
        }

        accxs[i] *= qs[i];
        accys[i] *= qs[i];
        acczs[i] *= qs[i];

        if constexpr(options == 1 || options == 3)
        {
            potentials[i] = potential;
        }

        if constexpr(options == 2 || options == 3)
        {
            energy += qs[i] * potential;
        }

    }
}


struct Options
{
    constexpr static int Pure = 0;
    constexpr static int ComputePotentials = 1;
    constexpr static int ComputeEnergy = 2;
    constexpr static int ComputePotentialsAndEnergy = 3;
};


/** @brief Class to compute the acceleration of interactions
    of charged particles
    @author Janos Meny
    @date August 2019
    */
template<typename T, int options>
class CoulombSolver : public Options
{
public:

    using accelerations_t = std::conditional_t<options == 1 || options == 3, Points4<T>, Points3<T>>;

    explicit CoulombSolver(std::shared_ptr<Points4<T>> points):
            m_points(points),
            m_accelerations(std::make_shared<accelerations_t>(points->size())){}

    /** Computes accelerations.
        @param enum - ComputePotentials, if this is true, in the
        the forth component will contain also the force potentials.
        @return a vector of Vector4f
        */
    void computeAccelerations()
    {
        int r, s;
        MPI_Comm_size(MPI_COMM_WORLD, &s);
        MPI_Comm_rank(MPI_COMM_WORLD, &r);

        int numPoints = m_points -> size();
        int begin = r * (numPoints / s + 1);
        int chunkSize = r == (s - 1) ? numPoints - begin : (numPoints / s + 1);
        int end = begin + chunkSize;

        auto [xs, ys, zs, qs] = m_points->data();

        T *accxs, *accys, *acczs, *potentials = nullptr;
        if constexpr(options == ComputePotentialsAndEnergy || options == ComputePotentials)
            std::tie(accxs, accys, acczs, potentials) = m_accelerations->data();
        else
            std::tie(accxs, accys, acczs) = m_accelerations->data();

        compute_accelerations<T, options>(
                xs, ys, zs, qs,
                accxs, accys, acczs,
                potentials,
                m_energy,
                numPoints, begin, end,
                m_numThreads);

        if constexpr(options == ComputePotentialsAndEnergy || options == ComputeEnergy)
        {
            MPI_Allreduce(&m_energy, &m_energy, 1, typeToMPI<T>(), MPI_SUM, MPI_COMM_WORLD);
            m_energy *= T{0.5};
        }


    }

private:

    int m_numThreads = std::thread::hardware_concurrency();

    T m_energy;

    std::shared_ptr<Points4<T>> m_points;
    std::shared_ptr<accelerations_t> m_accelerations;
};

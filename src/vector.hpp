

#pragma once

#include <array>
#include <cmath>

template<typename T, int n>
struct Vector
{
    template<typename ... Ts>
    constexpr explicit Vector(Ts ... ts): data{ts...}
    {
    }

    [[nodiscard]] constexpr T operator[](const int index) const noexcept
    {
        return data[index];
    }

    [[nodiscard]] constexpr T& operator[](const int index) noexcept
    {
        return data[index];
    }

    [[nodiscard]] constexpr int size() const noexcept
    {
        return n;
    }

    [[nodiscard]] constexpr T norm() const noexcept
    {
        T norm{0};
        for (int i = 0; i < n; ++i) {
            norm += data[i] * data[i];
        }
        return std::sqrt(norm);
    }
private:
    std::array<T, n> data;
};

template<typename T, int n>
Vector<T,n> operator-(const Vector<T,n>& a, const Vector<T,n>& b)
{
    Vector<T, n> c;
    for (int i = 0; i < n; ++i) {
        c[i] = a[i] - b[i];
    }
    return c;
}

template<typename T, int n>
Vector<T,n> operator+(const Vector<T,n>& a, const Vector<T,n>& b)
{
    Vector<T, n> c;
    for (int i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
    return c;
}

template<typename T, int n>
void operator+=(Vector<T,n>& a, const Vector<T,n>& b)
{
    a = a + b;
}

template<typename T, int n>
void operator-=(Vector<T,n>& a, const Vector<T,n>& b)
{
    a = a - b;
}

template<typename T, int n>
Vector<T,n> operator*(T scalar, const Vector<T,n>& b)
{
    Vector<T, n> c;
    for (int i = 0; i < n; ++i) {
        c[i] = scalar * b[i];
    }
    return c;
}

using Vector3f = Vector<float, 3>;
using Vector4f = Vector<float, 4>;

using Vector3d = Vector<double, 3>;
using Vector4d = Vector<double, 4>;

using Vector3i = Vector<int, 3>;
using Vector4i = Vector<int, 4>;


/*
#pragma once


#include <array>
#include <utility>

template<typename Derived>
class Base
{
public:
    using Scalar = typename Derived::Scalar;

    constexpr Scalar& operator[](const int index)
    {
        return static_cast<Derived*>(this)[index];
    }

    constexpr Scalar normSquared()
    {
        Scalar ns{0};

        for (int i = 0; i < this -> size(); ++i)
        {
            ns += (*this)[i] * (*this)[i];
        }
    }

    constexpr Scalar norm()
    {
        return sqrt(this->normSquared());
    }
};



template<typename T, int n, int alignement = 16>
class alignas(alignement) Vector : public Base<Vector<T, n, alignement>>
{
public:

    using Scalar = T;

    template<typename ... Ts>
    constexpr explicit Vector(Ts ... ts)
    {
        data = {ts...};
    }

    template<typename Expr>
    constexpr Vector(const Expr& expr)
    {
        unroll([&](int i){ data[i] = expr[i];}, expr);
    }

    constexpr T& operator[](const int index) const
    {
        return data[index];
    }

    constexpr int size()
    {
        return n;
    }


private:
    std::array<T, n> data;
};






*/

//
// Created by janos on 8/5/19.
//

#pragma once

#include "vector.hpp"

#include <cstdint>

enum OperationType : std::uint8_t
{
    PLUS,
    MINUS
};

template<typename Expr1, typename Expr2, OperationType operation>
struct BinaryOp : public Base<BinaryOp<Expr1, Expr2, operation>>
{
    using Scalar = typename Expr1::Scalar;

    constexpr Scalar& operator[](const int index)
    {
        if constexpr (operation == PLUS)
            return expr1[index] + expr2[index];
        else if constexpr(operation == MINUS)
            return expr1[index] - expr2[index];
    }

    const Expr1& expr1;
    const Expr2& expr2;

};

template<typename Expr1, typename Expr2>
constexpr BinaryOp<Expr1, Expr2, PLUS> operator+(const Expr1& expr1, const Expr2& expr2)
{
    return BinaryOp{expr1, expr2};
}
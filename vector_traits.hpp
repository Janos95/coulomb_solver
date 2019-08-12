//
// Created by janos on 8/5/19.
//

#pragma once

#include "vector.hpp"
#include "binary_operations.hpp"

#include <type_traits>



template<typename Expr>
class ExprSize {}; //undefined

template<typename T, int n>
class ExprSize<Vector<T, n>> : public std::integral_constant<int, n> {};

template<typename Expr1, typename Expr2, OperationType operation>
class ExprSize<BinaryOp<Expr1, Expr2, operation>> : public ExprSize<Expr1> {};

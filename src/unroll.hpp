//
// Created by janos on 8/5/19.
//

#pragma once

#include "vector_traits.hpp"

#include <utility>


template<typename Function, typename Expr, typename std::size_t ...I>
void constexpr mapImpl(Function f, Expr expr, std::index_sequence<I...>)
{
    (f(expr[I]), ...);
}

template<typename Function, typename Expr>
void constexpr map(Function f, Expr expr)
{
    mapImpl(f, expr, std::make_index_sequence<ExprSize<Expr>::value>{});
}

template<typename Function, typename Expr, typename std::size_t ...I>
void constexpr foldImpl(Function f, Expr expr, std::index_sequence<I...>)
{
    (f(expr[I]), ...);
}

template<typename Function, typename Expr>
void constexpr fold(Function f, Expr expr)
{
    foldImpl(f, expr, std::make_index_sequence<ExprSize<Expr>::value>{});
}


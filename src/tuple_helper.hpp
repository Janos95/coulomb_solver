//
// Created by janos on 18.08.19.
//

#pragma once

#include <tuple>
#include <utility>

namespace detail
{

    template<class F, class T>
    struct returns_void : public std::true_type
    {
    };

    template<class F, class ...T>
    struct returns_void<F, std::tuple<T...>>
    {
        constexpr static auto value = std::is_same_v<std::invoke_result_t<F, T...>, void>;
    };

    template<std::size_t i, class... Tuples>
    constexpr decltype(auto) get_ith_elements(Tuples &&...ts)
    {
        return std::make_tuple(std::get<i>(ts)...);
    }

    template<class F, class... Tuples, std::size_t... I>
    constexpr decltype(auto) apply_for_each_row_impl(F &&f, std::index_sequence<I...>, Tuples &&...ts)
    {
        if constexpr((returns_void<F, decltype(get_ith_elements<I>(ts...))>::value || ... || false))
        {
            (std::apply(f, get_ith_elements<I>(ts...)), ...);
            return;
        }
        else
        {
            return std::make_tuple(std::apply(f, get_ith_elements<I>(ts...))...);
        }
    }

} //detail

template <class F, class... Tuples>
constexpr decltype(auto) apply_for_each_row(F&& f, Tuples&& ...ts)
{
    //we'll assume that all tuples have the same size, TODO: check this?
    using Tuple1_t = decltype(std::get<0>(std::make_tuple(ts...)));
    constexpr auto size = std::tuple_size_v<std::remove_reference_t<Tuple1_t>>;

    return detail::apply_for_each_row_impl(
            std::forward<F>(f),
            std::make_index_sequence<size>{},
            std::forward<Tuples>(ts)...);
}
#pragma once

#include "tuple_helper.hpp"
//
// Created by janos on 18.08.19.
//

#include <vector>
#include <tuple>
#include <functional>


template<typename... T>
struct SoAIterator
{
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = std::tuple<T...>;
    using difference_type   = int;
    using reference         = std::tuple<T&...>;
    using pointer           = void;

    reference operator*()
    {
        return std::apply([](auto ...x){ std::make_tuple((*x)...);} , m_it);
    }

    reference operator[](difference_type n)
    {
        return *(this + n);
    }


    //this should be protected but i need to be able to get the tuple for the insert method
    std::tuple<typename std::vector<T>::iterator...> m_it;
};

template<typename... T>
SoAIterator<T...> operator+(SoAIterator<T...> it, typename SoAIterator<T...>::difference_type n)
{
    return std::apply([n](auto ...x){ return std::make_tuple((x+n)...);} , it.m_it);
}

template<typename... T>
SoAIterator<T...> operator-(SoAIterator<T...> it, typename SoAIterator<T...>::difference_type n)
{
    return std::apply([n](auto ...x){ return std::make_tuple((x-n)...);} , it.m_it);
}

template<typename... T>
typename SoAIterator<T...>::difference_type operator-(SoAIterator<T...> it1, SoAIterator<T...> it2)
{
    return std::get<0>(it1.m_it) - std::get<0>(it2.m_it);
}

template<typename... T>
SoAIterator<T...>& operator+=(SoAIterator<T...>& it, typename SoAIterator<T...>::difference_type n)
{
    return it - n;
}

template<typename... T>
SoAIterator<T...>& operator-=(SoAIterator<T...>& it, typename SoAIterator<T...>::difference_type n)
{
    return it + n;
}

template<typename... T>
typename SoAIterator<T...>::diference_type operator<(SoAIterator<T...> a, SoAIterator<T...> b)
{
    return b - a > 0;
}

template<typename... T>
typename SoAIterator<T...>::diference_type operator>(SoAIterator<T...> a, SoAIterator<T...> b)
{
    return b < a;
}

template<typename... T>
typename SoAIterator<T...>::diference_type operator<=(SoAIterator<T...> a, SoAIterator<T...> b)
{
    return !(a > b);
}

template<typename... T>
typename SoAIterator<T...>::diference_type operator>=(SoAIterator<T...> a, SoAIterator<T...> b)
{
    return !(a < b);
}


template<typename... T>
class SoA
{
public:

    using iterator = SoAIterator<T...>;

    SoA<T...>() = default;

    explicit SoA<T...>(std::size_t count)
    {
        std::apply([count](auto&& ...xs){ (xs.resize(count), ...);} , m_vectors);
    }

    void push_back(T... x)
    {
        apply_for_each_row([](auto&& xs, auto x){ xs.push_back(x);} , m_vectors, std::make_tuple(x...));
    }


    iterator begin()
    {
        return {std::apply([](auto&& ...xs){ return std::make_tuple(xs.begin()...);} , m_vectors)};
    }

    iterator end()
    {
        return {std::apply([](auto&& ...xs){ return std::make_tuple(xs.end()...);} , m_vectors)};
    }

    auto data()
    {
        return std::apply([](auto&& ...xs){ return std::make_tuple(xs.data()...);} , m_vectors);
    }

    std::size_t size()
    {
        return (this -> end() - this -> begin());
    }

    iterator insert(iterator pos, iterator first, iterator last)
    {
        return apply_for_each_row([](auto&& xs, auto p, auto f, auto l){ return xs.insert(p, f, l);} , m_vectors, pos.m_it, first.m_it, last.m_it);
    }

private:
    std::tuple<std::vector<T>...> m_vectors;
};


template<typename T>
using Points4 = SoA<T, T, T, T>;

template<typename T>
using Points3 = SoA<T, T, T>;



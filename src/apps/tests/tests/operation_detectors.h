#pragma once


#include "detection.h"


namespace ad {


template<class T, class U>
using is_additive_t = decltype(std::declval<T&>() + std::declval<U&>());
template<class T, class U>
using is_additivecompound_t = decltype(std::declval<T&>() += std::declval<U&>());

template<class T, class U>
using is_substractive_t = decltype(std::declval<T&>() - std::declval<U&>());
template<class T, class U>
using is_substractivecompound_t = decltype(std::declval<T&>() -= std::declval<U&>());

template <class T, class U>
using is_multiplicative_t = decltype(std::declval<T&>() * std::declval<U&>());
template <class T, class U>
using is_multiplicative_assignable_t = decltype(std::declval<T&>() *= std::declval<U&>());

template <class T, class U>
using is_divisive_t = decltype(std::declval<T&>() / std::declval<U&>());
template <class T, class U>
using is_divisive_assignable_t = decltype(std::declval<T&>() /= std::declval<U&>());

template <class T, class U>
using is_cwmul_t = decltype(std::declval<T&>().cwMul(std::declval<U&>()));
template <class T, class U>
using is_cwmul_assignable_t = decltype(std::declval<T&>().cwMulAssign(std::declval<U&>()));

template <class T, class U>
using is_cwdiv_t = decltype(std::declval<T&>().cwDiv(std::declval<U&>()));
template <class T, class U>
using is_cwdiv_assignable_t = decltype(std::declval<T&>().cwDivAssign(std::declval<U&>()));

template <class From, class To>
using is_staticcastable_t = decltype(static_cast<To>(std::declval<From>()));


} // namespace ad

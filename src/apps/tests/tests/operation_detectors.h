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

} // namespace ad


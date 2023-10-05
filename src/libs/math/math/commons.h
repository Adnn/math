#pragma once


#include <cmath>
namespace ad {
namespace math {


typedef double real_number;

template<class T_number>
bool epsilonCompare(T_number a, T_number b)
{
    return std::fabs(a - b ) < std::numeric_limits<T_number>::epsilon();
}

}} // namespace ad::math


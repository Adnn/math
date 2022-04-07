#pragma once

#include "Interpolation.h"

#include "../Angle.h"
#include "../Quaternion.h"


namespace ad {
namespace math {


/// \brief Quaternion linear interpolation, along the shortest path
template <class T_number, class T_parameter>
Quaternion<T_number> lerp(Quaternion<T_number> aLhs,
                          Quaternion<T_number> aRhs,
                          const Clamped<T_parameter> & aParameter)
noexcept(decltype(aLhs)::should_noexcept)
{
    // TODO is there a cheaper test in this situation (as we won't need the cosine later).
    if (getCosineBetween(aLhs, aRhs) < 0)
    {
        aLhs = -aLhs;
    }
    Vec<4, T_number> interpolated = lerp(aLhs.asVec(), aRhs.asVec(), aParameter).normalize();
    return {interpolated.x(), interpolated.y(), interpolated.z(), interpolated.w()};
}


/// \brief Quaternion spherical liner interpolation, along the shortest path
template <class T_number, class T_parameter>
Quaternion<T_number> slerp(Quaternion<T_number> aLhs,
                           Quaternion<T_number> aRhs,
                           const Clamped<T_parameter> & aParameter)
noexcept(decltype(aLhs)::should_noexcept)
{
    T_number cosine = getCosineBetween(aLhs, aRhs);
    if (cosine < 0)
    {
        aLhs = -aLhs;
        cosine = -cosine;
    }

    T_parameter lhsParam;
    T_parameter rhsParam;

    // If the two quaternions are too close, division by sin(theta) might divide by zero.
    // Fall back to linear interpolation.
    // See: 3MPGGD p262
    if (cosine > T_number{0.9999})
    {
        lhsParam = T_parameter{1} - aParameter;
        rhsParam = aParameter;
    }
    else
    {
        Angle theta = acos<Radian_tag>(cosine);
        lhsParam = sin((T_parameter{1} - aParameter) * theta) / sin(theta);
        rhsParam = sin(aParameter * theta) / sin(theta);
    }
    Vec<4, T_number> interpolated = lhsParam * aLhs.asVec() + rhsParam * aRhs.asVec();
    return {interpolated.x(), interpolated.y(), interpolated.z(), interpolated.w()};
}


} // namespace math
} // namespace ad

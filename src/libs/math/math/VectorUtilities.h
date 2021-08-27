#pragma once


#include "Angle.h"
#include "Vector.h"


namespace ad {
namespace math {


/// \return The angle to rotate `a` onto `b`, in the range [-revolution, +revolution].
///  Positive is counter-clockwise.
template <class T_angle = Radian<double>, class T_vecLeft, class T_vecRight>
T_angle getOrientedAngle(T_vecLeft a, T_vecRight b)
{
    static_assert(a.Dimension == 2 && b.Dimension == 2,
                  "Angle must be between two vectors of dimension 2.");
    return T_angle{ std::atan2(b.y(), b.x()) - std::atan2(a.y(), a.x()) };
}


} // namespace math
} // namespace ad

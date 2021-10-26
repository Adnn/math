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


/// \brief Factory for Size<2, T> from a height and the aspect ratio.
/// \param aAspectRatio is the ratio width/height.
template <class T_number, class T_ratio>
Size<2, T_number> makeSizeFromHeight(T_number aHeight, T_ratio aAspectRatio)
{
    return Size<2, T_number>{
        static_cast<T_number>(aHeight * aAspectRatio),
        aHeight
    };
}


/// \brief Factory for Size<2, T> from a width and the aspect ratio.
/// \param aAspectRatio is the ratio width/height.
template <class T_number, class T_ratio>
Size<2, T_number> makeSizeFromWidth(T_number aWidth, T_ratio aAspectRatio)
{
    return Size<2, T_number>{
        aWidth,
        static_cast<T_number>(aWidth / aAspectRatio)
    };
}


/// \brief Ratio of width over height.
template <class T_ratio, class T_number>
T_ratio getRatio(const Size<2, T_number> aSize)
{
    return static_cast<T_ratio>(aSize.width()) / aSize.height();
}


} // namespace math
} // namespace ad

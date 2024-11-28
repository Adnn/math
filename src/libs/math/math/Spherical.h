#pragma once


#include "Angle.h"
#include "Base.h"
#include "Vector.h"

#include <cassert>


namespace ad {
namespace math {


template <std::floating_point T_value>
class Spherical
{
public:
    template <class T_polarUnitTag = Radian_tag, class T_azimuthalUnitTag = Radian_tag>
    explicit constexpr Spherical(
        T_value aRadius,
        Angle<T_value, T_polarUnitTag> aPolar = Radian<T_value>{pi<T_value> / 2.},
        Angle<T_value, T_azimuthalUnitTag> aAzimuthal = Radian<T_value>{0.}) noexcept;

    constexpr Position<3, T_value> toCartesian() const noexcept;

    constexpr T_value & radius() noexcept
    { return mRadius; }
    constexpr Radian<T_value> & polar() noexcept
    { return mPolar; }
    constexpr Radian<T_value> & azimuthal() noexcept
    { return mAzimuthal; }

    constexpr T_value radius() const noexcept
    { return mRadius; }
    constexpr Radian<T_value> polar() const noexcept
    { return mPolar; }
    constexpr Radian<T_value> azimuthal() const noexcept
    { return mAzimuthal; }

    constexpr Frame<3, T_value> computeTangentFrame() const;

    template<class T_witness>
    void describeTo(T_witness && aWitness)
    {
    }

private:
    T_value mRadius{0.};
    // Polar angle measured from Y.
    Radian<T_value> mPolar{pi<T_value> / 2.};
    // Azimuthal angle in the plane {X, Z}, measured CCW from X.
    Radian<T_value> mAzimuthal{0.};
};


//
// Implementation
//

template <std::floating_point T_value>
template <class T_polarUnitTag, class T_azimuthalUnitTag>
constexpr Spherical<T_value>::Spherical(
    T_value aRadius,
    Angle<T_value, T_polarUnitTag> aPolar,
    Angle<T_value, T_azimuthalUnitTag> aAzimuthal) noexcept :
        mRadius{aRadius},
        mPolar{aPolar},
        mAzimuthal{aAzimuthal}
{}


template <std::floating_point T_value>
constexpr Position<3, T_value> Spherical<T_value>::toCartesian() const noexcept
{
    return {
        mRadius * sin(mAzimuthal) * sin(mPolar),
        mRadius * cos(mPolar),
        mRadius * cos(mAzimuthal) * sin(mPolar),
    };
}


// TODO Research state of the art spherical to tagent base, this is a naive implementation.
template <std::floating_point T_value>
constexpr Frame<3, T_value> Spherical<T_value>::computeTangentFrame() const
{
    // Maybe there can be a meaning for non-zero negative radii,
    // but we simply do not support them atm.
    assert(mRadius > 0);

    Vec<3, T_value> w = toCartesian().template as<Vec>();
    Vec<3, T_value> u{cos(azimuthal()), T_value{0}, -sin(azimuthal())};
    return Frame<3, T_value>{
        .origin = w.template as<Position>(),
        .base = OrthonormalBase<3, T_value>::MakeFromWSide(w, u)
    };
}


} // namespace math
} // namespace ad

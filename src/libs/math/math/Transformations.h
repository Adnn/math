#pragma once

#include "commons.h"

#include "Angle.h"
#include "Homogeneous.h"
#include "Matrix.h"
#include "Rectangle.h"
#include "Vector.h"


namespace ad {
namespace math {


// Note: C++ identifier cannot start with a number
namespace trans2d {


    template <class T_number, class T_angleUnitTag=void>
    constexpr Matrix<2, 2, T_number> rotate(const Angle<T_number, T_angleUnitTag> aAngle);


    template <class T_number>
    constexpr Matrix<2, 2, T_number> scale(const T_number aFactorX, const T_number aFactorY);

    template <class T_number>
    constexpr Matrix<2, 2, T_number> scale(const T_number aFactor, const UnitVec<2, T_number> aAxis);


    /// \brief Non-uniform non-axis-aligned scale.
    ///
    /// The scaling axis are X and Y, but rotated counter-clockwise by `aCounterClockwise`.
    ///
    /// \note see: FoCG 3rd p122
    template <class T_number, class T_angleRepresentation, class T_angleUnitTag>
    constexpr Matrix<2, 2, T_number>
    scale(const T_number aFactorHorizontal, const T_number aFactorVertical,
          const Angle<T_angleRepresentation, T_angleUnitTag> aCounterClockwise);


    /// \brief Projects \b onto X
    template <class T_number=real_number>
    constexpr Matrix<2, 2, T_number> projectOrthographicOntoX();

    /// \brief Projects \b onto Y
    template <class T_number=real_number>
    constexpr Matrix<2, 2, T_number> projectOrthographicOntoY();

    /// \brief Projects \b along aAxis, \b onto the line perpendicular to axis
    template <class T_number>
    constexpr Matrix<2, 2, T_number> projectOrthographicAlong(const UnitVec<2, T_number> aAxis);


    /// \brief Reflects \b along X
    template <class T_number=real_number>
    constexpr Matrix<2, 2, T_number> reflectAlongX();

    /// \brief Reflects \b along Y
    template <class T_number=real_number>
    constexpr Matrix<2, 2, T_number> reflectAlongY();

    /// \brief Reflects \b along aAxis
    template <class T_number>
    constexpr Matrix<2, 2, T_number> reflectAlong(const UnitVec<2, T_number> aAxis);


    template <class T_number>
    constexpr Matrix<2, 2, T_number> shearX(const T_number aWeightY);

    template <class T_number>
    constexpr Matrix<2, 2, T_number> shearY(const T_number aWeightX);

    /// \brief Shear matrix that tilts the vertical axis counter-clockwise (shears the X axis).
    template <class T_number=real_number, class T_angleRepresentation, class T_angleUnitTag>
    constexpr Matrix<2, 2, T_number>
    shearVertical(const Angle<T_angleRepresentation, T_angleUnitTag> aCounterClockwise);

    /// \brief Shear matrix that tilts the horizontal axis counter-clockwise (shear the Y axis).
    template <class T_number=real_number, class T_angleRepresentation, class T_angleUnitTag>
    constexpr Matrix<2, 2, T_number>
    shearHorizontal(const Angle<T_angleRepresentation, T_angleUnitTag> aCounterClockwise);


    template <class T_number>
    constexpr AffineMatrix<3, T_number>
    translate(const Vec<2, T_number> aDisplacement);


    template <class T_number>
    constexpr AffineMatrix<3, T_number>
    window(const Rectangle<T_number> aSource, const Rectangle<T_number> aDestination);


} // namespace trans2d


namespace trans3d {


    template <class T_number, class T_angleUnitTag=void>
    constexpr Matrix<3, 3, T_number> rotateX(const Angle<T_number, T_angleUnitTag> aAngle);

    template <class T_number, class T_angleUnitTag=void>
    constexpr Matrix<3, 3, T_number> rotateY(const Angle<T_number, T_angleUnitTag> aAngle);

    template <class T_number, class T_angleUnitTag=void>
    constexpr Matrix<3, 3, T_number> rotateZ(const Angle<T_number, T_angleUnitTag> aAngle);

    template <class T_number, class T_angleUnitTag=void>
    constexpr Matrix<3, 3, T_number> rotate(const Angle<T_number, T_angleUnitTag> aAngle,
                                            const UnitVec<3, T_number> aAxis);


    template <class T_number>
    constexpr Matrix<3, 3, T_number> scale(const T_number aFactorX,
                                           const T_number aFactorY,
                                           const T_number aFactorZ);

    template <class T_number>
    constexpr Matrix<3, 3, T_number> scale(const T_number aFactor, const UnitVec<3, T_number> aAxis);


    /// \brief Projects \b onto XY plane
    template <class T_number=real_number>
    constexpr Matrix<3, 3, T_number> projectOrthographicOntoXY();

    /// \brief Projects \b onto XZ plane
    template <class T_number=real_number>
    constexpr Matrix<3, 3, T_number> projectOrthographicOntoXZ();

    /// \brief Projects \b onto YZ plane
    template <class T_number=real_number>
    constexpr Matrix<3, 3, T_number> projectOrthographicOntoYZ();

    /// \brief Projects \b along aAxis, onto the plane perpendicular to it
    template <class T_number>
    constexpr Matrix<3, 3, T_number> projectOrthographicAlong(const UnitVec<3, T_number> aAxis);


    /// \brief Reflects \b along X
    template <class T_number=real_number>
    constexpr Matrix<3, 3, T_number> reflectAlongX();

    /// \brief Reflects \b along Y
    template <class T_number=real_number>
    constexpr Matrix<3, 3, T_number> reflectAlongY();

    /// \brief Reflects \b along Z
    template <class T_number=real_number>
    constexpr Matrix<3, 3, T_number> reflectAlongZ();

    /// \brief Reflects \b along aAxis
    template <class T_number>
    constexpr Matrix<3, 3, T_number> reflectAlong(const UnitVec<3, T_number> aAxis);


    template <class T_number>
    constexpr Matrix<3, 3, T_number> shearXY(const T_number aWeightZonX,
                                             const T_number aWeightZonY);

    template <class T_number>
    constexpr Matrix<3, 3, T_number> shearXZ(const T_number aWeightYonX,
                                             const T_number aWeightYonZ);

    template <class T_number>
    constexpr Matrix<3, 3, T_number> shearYZ(const T_number aWeightXonY,
                                             const T_number aWeightXonZ);


    template <class T_number>
    constexpr AffineMatrix<4, T_number>
    translate(const Vec<3, T_number> aDisplacement);


} // namespace trans3d


} // namespace math
} // namespace ad


#include "Transformations-impl.h"

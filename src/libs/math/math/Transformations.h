#pragma once

#include "commons.h"

#include "Angle.h"
#include "Base.h"
#include "Box.h"
#include "Homogeneous.h"
#include "LinearMatrix.h"
#include "Rectangle.h"
#include "Vector.h"


namespace ad {
namespace math {


// Note: C++ identifier cannot start with a number
namespace trans2d {


    /// \brief Counter-clockwise rotation.
    template <class T_number, class T_angleUnitTag=void>
    constexpr LinearMatrix<2, 2, T_number> rotate(const Angle<T_number, T_angleUnitTag> aAngle);

    template <class T_number, class T_angleUnitTag=void>
    constexpr AffineMatrix<3, T_number> rotateAbout(const Angle<T_number, T_angleUnitTag> aAngle,
                                                    const Position<2, T_number> aRotationCenter);

    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> scale(const T_number aFactorX, const T_number aFactorY);

    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> scale(Size<2, T_number> aFactors);

    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> scale(const T_number aFactor, const UnitVec<2, T_number> aAxis);

    /// \brief Non-uniform non-axis-aligned scale.
    ///
    /// The scaling axis are X and Y, but rotated counter-clockwise by `aCounterClockwise`.
    ///
    /// \note see: FoCG 3rd p122
    template <class T_number, class T_angleRepresentation, class T_angleUnitTag>
    constexpr LinearMatrix<2, 2, T_number>
    scale(const T_number aFactorHorizontal, const T_number aFactorVertical,
          const Angle<T_angleRepresentation, T_angleUnitTag> aCounterClockwise);

    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> scaleUniform(const T_number aFactor);

    /// \brief Projects \b onto X
    template <class T_number=real_number>
    constexpr LinearMatrix<2, 2, T_number> projectOrthographicOntoX();

    /// \brief Projects \b onto Y
    template <class T_number=real_number>
    constexpr LinearMatrix<2, 2, T_number> projectOrthographicOntoY();

    /// \brief Projects \b along aAxis, \b onto the line perpendicular to axis
    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> projectOrthographicAlong(const UnitVec<2, T_number> aAxis);


    /// \brief Reflects \b along X
    template <class T_number=real_number>
    constexpr LinearMatrix<2, 2, T_number> reflectAlongX();

    /// \brief Reflects \b along Y
    template <class T_number=real_number>
    constexpr LinearMatrix<2, 2, T_number> reflectAlongY();

    /// \brief Reflects \b along aAxis
    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> reflectAlong(const UnitVec<2, T_number> aAxis);


    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> shearX(const T_number aWeightY);

    template <class T_number>
    constexpr LinearMatrix<2, 2, T_number> shearY(const T_number aWeightX);

    /// \brief Shear matrix that tilts the vertical axis counter-clockwise (shears the X axis).
    template <class T_number=real_number, class T_angleRepresentation, class T_angleUnitTag>
    constexpr LinearMatrix<2, 2, T_number>
    shearVertical(const Angle<T_angleRepresentation, T_angleUnitTag> aCounterClockwise);

    /// \brief Shear matrix that tilts the horizontal axis counter-clockwise (shear the Y axis).
    template <class T_number=real_number, class T_angleRepresentation, class T_angleUnitTag>
    constexpr LinearMatrix<2, 2, T_number>
    shearHorizontal(const Angle<T_angleRepresentation, T_angleUnitTag> aCounterClockwise);


    template <class T_number>
    constexpr AffineMatrix<3, T_number>
    translate(const Vec<2, T_number> aDisplacement);


    template <class T_number>
    constexpr AffineMatrix<3, T_number>
    window(const Rectangle<T_number> aSource, const Rectangle<T_number> aDestination);


    // 
    // Coordinate transformations
    //
    template <class T_number>
    constexpr AffineMatrix<3, T_number>
    frameToCanonical(const Frame<2, T_number> aFrame);


    template <class T_number>
    constexpr AffineMatrix<3, T_number>
    canonicalToFrame(const Frame<2, T_number> aFrame);


    //
    // Viewing (higher level) transformations
    //
    /// \brief Project a rectangle onto the canonical unit square [-1, 1]^2 (i.e. OpenGL's)
    template <class T_number>
    constexpr AffineMatrix<3, T_number>
    orthographicProjection(const Rectangle<T_number> aProjected);


} // namespace trans2d


namespace trans3d {


    template <class T_number, class T_angleUnitTag=void>
    constexpr LinearMatrix<3, 3, T_number> rotateX(const Angle<T_number, T_angleUnitTag> aAngle);

    template <class T_number, class T_angleUnitTag=void>
    constexpr LinearMatrix<3, 3, T_number> rotateY(const Angle<T_number, T_angleUnitTag> aAngle);

    template <class T_number, class T_angleUnitTag=void>
    constexpr LinearMatrix<3, 3, T_number> rotateZ(const Angle<T_number, T_angleUnitTag> aAngle);

    template <class T_number, class T_angleUnitTag=void>
    constexpr LinearMatrix<3, 3, T_number> rotate(const UnitVec<3, T_number> aAxis,
                                                  const Angle<T_number, T_angleUnitTag> aAngle);


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> scale(const T_number aFactorX,
                                                 const T_number aFactorY,
                                                 const T_number aFactorZ);

    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> scale(Size<3, T_number> aFactors);


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> scale(const T_number aFactor, const UnitVec<3, T_number> aAxis);

    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> scaleUniform(const T_number aFactor);

    /// \brief Projects \b onto XY plane
    template <class T_number=real_number>
    constexpr LinearMatrix<3, 3, T_number> projectOrthographicOntoXY();

    /// \brief Projects \b onto XZ plane
    template <class T_number=real_number>
    constexpr LinearMatrix<3, 3, T_number> projectOrthographicOntoXZ();

    /// \brief Projects \b onto YZ plane
    template <class T_number=real_number>
    constexpr LinearMatrix<3, 3, T_number> projectOrthographicOntoYZ();

    /// \brief Projects \b along aAxis, onto the plane perpendicular to it
    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> projectOrthographicAlong(const UnitVec<3, T_number> aAxis);


    /// \brief Reflects \b along X
    template <class T_number=real_number>
    constexpr LinearMatrix<3, 3, T_number> reflectAlongX();

    /// \brief Reflects \b along Y
    template <class T_number=real_number>
    constexpr LinearMatrix<3, 3, T_number> reflectAlongY();

    /// \brief Reflects \b along Z
    template <class T_number=real_number>
    constexpr LinearMatrix<3, 3, T_number> reflectAlongZ();

    /// \brief Reflects \b along aAxis
    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> reflectAlong(const UnitVec<3, T_number> aAxis);


    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> shearXY(const T_number aWeightZonX,
                                                   const T_number aWeightZonY);

    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> shearXZ(const T_number aWeightYonX,
                                                   const T_number aWeightYonZ);

    template <class T_number>
    constexpr LinearMatrix<3, 3, T_number> shearYZ(const T_number aWeightXonY,
                                                   const T_number aWeightXonZ);


    template <class T_number>
    constexpr AffineMatrix<4, T_number>
    translate(const Vec<3, T_number> aDisplacement);


    template <class T_number>
    constexpr AffineMatrix<4, T_number>
    window(const Box<T_number> aSource, const Box<T_number> aDestination);


    // 
    // Coordinate transformations
    //
    template <class T_number>
    constexpr AffineMatrix<4, T_number>
    frameToCanonical(const Frame<3, T_number> aFrame);


    template <class T_number>
    constexpr AffineMatrix<4, T_number>
    canonicalToFrame(const Frame<3, T_number> aFrame);

    //
    // Viewing (higher level) transformations
    //
    /// \brief Project a box onto the canonical unit cube [-1, 1]^3 (i.e. OpenGL's)
    template <class T_number>
    constexpr AffineMatrix<4, T_number>
    orthographicProjection(const Box<T_number> aProjected);


    /// \brief Perspective matrix.
    /// \see FoCG 3rd - 7.3 Perspective Projection (p 151)
    /// \important: The matrix is negated compared to what is presented in FoCG
    /// because otherwise the homogeneous (w) values of multiplied vectors will be negative,
    /// which prevents the usual clipping test -w < x, y, z< w.
    template <class T_number>
    constexpr Matrix<4, 4, T_number>
    perspective(const T_number aNearPlaneZ, const T_number aFarPlaneZ);


    /// \brief The inverse of the perspective matrix.
    template <class T_number>
    constexpr Matrix<4, 4, T_number>
    perspectiveInverse(const T_number aNearPlaneZ, const T_number aFarPlaneZ);


    /// \brief Project the unit cube [-1, 1]^3 (i.e. OpenGL's) onto the viewport, while remapping depth.
    template <class T_number>
    constexpr AffineMatrix<4, T_number>
    ndcToViewport(const Rectangle<T_number> aViewPort, T_number aNear, T_number aFar);


} // namespace trans3d


} // namespace math
} // namespace ad


#include "Transformations-impl.h"

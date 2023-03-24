#pragma once


#include "Angle.h"
#include "Clamped.h"
#include "LinearMatrix.h"
#include "Vector.h"


namespace ad {
namespace math {


/// \brief Intended to represent rotations, so unit quaternions.
template <class T_number>
class Quaternion
{
public:
    using value_type = T_number;

    // Implementer note:
    // This is definitely too conservative, but I don't feel confident in my understanding of
    // noexcept at the moment. (And this class is intended for arithmetic types)
    // Also, this should probably be different depending on the member function.
    static constexpr bool should_noexcept = std::is_arithmetic<value_type>::value;

    /// \attention The provided values must correspond to a unit quaternion!
    constexpr Quaternion(T_number aX, T_number aY, T_number aZ, T_number aW)
    noexcept(should_noexcept);

    template <class T_unitTag>
    constexpr Quaternion(UnitVec<3, T_number> aRotationAxis,
                         Angle<T_number, T_unitTag> aRotationAngle) noexcept(should_noexcept);

    /// \brief Constructs a zero-rotation unit quaternion.
    static constexpr Quaternion Identity() noexcept(should_noexcept);

    constexpr bool operator==(const Quaternion &aRhs) const noexcept(should_noexcept);
    constexpr bool operator!=(const Quaternion &aRhs) const noexcept(should_noexcept);

    constexpr T_number & x() noexcept(should_noexcept);
    constexpr T_number & y() noexcept(should_noexcept);
    constexpr T_number & z() noexcept(should_noexcept);
    constexpr T_number & w() noexcept(should_noexcept);

    constexpr T_number x() const noexcept(should_noexcept);
    constexpr T_number y() const noexcept(should_noexcept);
    constexpr T_number z() const noexcept(should_noexcept);
    constexpr T_number w() const noexcept(should_noexcept);

    constexpr Quaternion operator-() const noexcept(should_noexcept);

    constexpr Quaternion & operator*=(const Quaternion & aRhs) noexcept(should_noexcept);

    /// \brief Return antoher Quaternion, which is the conjugate of this.
    /// \note The conjugate represents the opposite angular displacement.
    /// \note For unit quaternions, the conjugate is the inverse.
    constexpr Quaternion conjugate() const noexcept(should_noexcept);

    /// \brief Return antoher Quaternion, which is the inverse of this.
    /// \note Multiplying a quaternion by its inverse yields the identity quaternion.
    constexpr Quaternion inverse() const noexcept(should_noexcept);

    // TODO Ad 2022/03/11: Implement the conversion to axis-angle.
    //template <class T_unitTag>
    //constexpr Angle<T_number, T_unitTag> rotationAngle() const noexcept(should_noexcept);
    //
    //constexpr UnitVec<3, T_number> rotationAxis() const noexcept(should_noexcept);

    template <class T_derived> 
    //requires (is_position_v<T_derived> || is_vec_v<T_derived>)
    T_derived rotate(const Vector<T_derived, 3, T_number> & aVector) noexcept(should_noexcept);

    constexpr LinearMatrix<3, 3, T_number> toRotationMatrix() const noexcept(should_noexcept);

    /// \brief Return true if elements of `aRhs` are all within the tolerance `aEpsilon` of corresponding
    /// elements in this quaternion.
    ///
    /// \note This can be used to test if two instances are almost equal.
    constexpr bool equalsWithinTolerance(const Quaternion & aRhs, T_number aEpsilon) const noexcept(should_noexcept);

    constexpr Vec<4, T_number> asVec() const noexcept(should_noexcept);

private:
    constexpr Quaternion(Vec<3, T_number> aVector, T_number aW)  noexcept(should_noexcept);

    Vec<3, T_number> mVector;
    T_number mW;
};


/// \brief Quaternion multiplication, composing rotations.
/// \warning Quaternion multiplication performs the rotations from right to left.
template <class T_number>
constexpr Quaternion<T_number>
operator*(Quaternion<T_number> aLhs, const Quaternion<T_number> & aRhs) 
noexcept(decltype(aLhs)::should_noexcept);


/// \brief Quaternion difference gives the angular
/// displacement from the orientation lhs to orientation rhs.
template <class T_number>
constexpr Quaternion<T_number>
difference(Quaternion<T_number> aLhs, const Quaternion<T_number> & aRhs) 
noexcept(decltype(aLhs)::should_noexcept);


template <class T_number>
T_number getCosineHalfAngle(Quaternion<T_number> aLhs, const Quaternion<T_number> & aRhs)
noexcept(decltype(aLhs)::should_noexcept);


/// \brief Formatted output operation
template <class T_number>
std::ostream & operator<<(std::ostream & aOut, const Quaternion<T_number> & aQuaternion);


} // namespace math
} // namespace ad


#include "Quaternion-impl.h"
#pragma once


#include "Angle.h"
#include "Quaternion.h"


namespace ad::math {


/// @brief Euler angles are three angle describing an orientation in 3-dimension.
/// @see https://en.wikipedia.org/wiki/Euler_angles
template <std::floating_point T_number, template <class> class TT_angle = Radian>
struct EulerAngles
{
    using value_type = T_number;

    template <template <class> class TT_targetAngle>
    constexpr EulerAngles<T_number, TT_targetAngle> as() const noexcept
    {
        return {
            x.template as<TT_targetAngle>(),
            y.template as<TT_targetAngle>(),
            z.template as<TT_targetAngle>(),
        };
    }

    constexpr EulerAngles operator-() const noexcept
    {
        return {
            -x,
            -y,
            -z,
        };
    }

    constexpr bool operator==(const EulerAngles &) const noexcept = default;


    /// \brief Return true if elements of `aRhs` are all within the tolerance `aEpsilon` of corresponding
    /// elements in this EulerAngle.
    ///
    /// \note This can be used to test if two instances are almost equal.
    constexpr bool equalsWithinTolerance(const EulerAngles & aRhs, T_number aEpsilon) const noexcept;

    // If we imagine a plane along the X axis, with up direction being Z axis (convention in 3D Math Primer), then:
    // x: roll
    // y: pitch
    // z: yaw
    TT_angle<T_number> x, y, z;
};


template <std::floating_point T_number, template <class> class TT_angle>
constexpr bool EulerAngles<T_number, TT_angle>::equalsWithinTolerance(
    const EulerAngles & aRhs,
    T_number aEpsilon) const noexcept
{
    return absoluteTolerance(x.value(), aRhs.x.value(), aEpsilon) 
     && absoluteTolerance(y.value(), aRhs.y.value(), aEpsilon)
     && absoluteTolerance(z.value(), aRhs.z.value(), aEpsilon);
     ;
}


/// @brief Canonize the EulerAngles so that all angles are in ]-revolution/2, +revolution/2]
template <class T_number, template <class> class TT_angle>
EulerAngles<T_number, TT_angle> reduce(EulerAngles<T_number, TT_angle> aLhs)
{
    return {
        .x = reduce(aLhs.x),
        .y = reduce(aLhs.y),
        .z = reduce(aLhs.z),
    };
}


// For conversion procedures see:
// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
template <class T_number, template <class> class TT_angle = Radian>
EulerAngles<T_number, TT_angle> toEulerAngles(Quaternion<T_number> q) 
{
    using Angle_t = TT_angle<T_number>;
    EulerAngles<T_number, TT_angle> angles;

    // roll (x-axis rotation)
    T_number sinr_cosp = 2 * (q.w() * q.x() + q.y() * q.z());
    T_number cosr_cosp = 1 - 2 * (q.x() * q.x() + q.y() * q.y());
    angles.x = Angle_t{std::atan2(sinr_cosp, cosr_cosp)};

    // pitch (y-axis rotation)
    T_number sinp = std::sqrt(1 + 2 * (q.w() * q.y() - q.x() * q.z()));
    T_number cosp = std::sqrt(1 - 2 * (q.w() * q.y() - q.x() * q.z()));
    angles.y = Angle_t{2 * std::atan2(sinp, cosp) - pi<T_number> / 2};

    // yaw (z-axis rotation)
    T_number siny_cosp = 2 * (q.w() * q.z() + q.x() * q.y());
    T_number cosy_cosp = 1 - 2 * (q.y() * q.y() + q.z() * q.z());
    angles.z = Angle_t{std::atan2(siny_cosp, cosy_cosp)};

    return angles;
}


// see: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Source_code
template <class T_number, template <class> class TT_angle>
Quaternion<T_number> toQuaternion(EulerAngles<T_number, TT_angle> aEuler)
{
    // Abbreviations for the various angular functions
    T_number cr = cos(aEuler.x  * 0.5f);
    T_number sr = sin(aEuler.x  * 0.5f);
    T_number cp = cos(aEuler.y * 0.5f);
    T_number sp = sin(aEuler.y * 0.5f);
    T_number cy = cos(aEuler.z   * 0.5f);
    T_number sy = sin(aEuler.z   * 0.5f);

    Vec<4, T_number> v{
        sr * cp * cy - cr * sp * sy,
        cr * sp * cy + sr * cp * sy,
        cr * cp * sy - sr * sp * cy,
        cr * cp * cy + sr * sp * sy,
    };
    v.normalize();

    return Quaternion<T_number>{
        v.x(),
        v.y(),
        v.z(),
        v.w(),
    };
}


template <class T_number, template <class> class TT_angle>
math::LinearMatrix<3, 3, T_number> toRotationMatrix(math::EulerAngles<T_number, TT_angle> aEuler)
{
    T_number cx = cos(aEuler.x);
    T_number sx = sin(aEuler.x);
    T_number cy = cos(aEuler.y);
    T_number sy = sin(aEuler.y);
    T_number cz = cos(aEuler.z);
    T_number sz = sin(aEuler.z);
    return {
        // 1                        // 2                        // 3
        cy * cz,                    cy * sz,                    -sy,
        -cx * sz + sx * sy * cz,    cx * cz + sx * sy * sz,     sx * cy,
        sx * sz + cx * sy * cz,     -sx * cz + cx * sy * sz,    cx * cy
    };
}


/*
 * Output operator
 */
template <class T_number, template <class> class TT_angle>
std::ostream & operator<<(std::ostream & aOut, const EulerAngles<T_number, TT_angle> & aEuler)
{
    return aOut
        << "{" << aEuler.x << "; " << aEuler.y << "; " << aEuler.z << "}"
        ;
}


} // namespace ad::math
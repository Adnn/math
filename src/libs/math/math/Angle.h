#pragma once

#include "Constants.h"

#include <string>

#include <cmath>


namespace ad {
namespace math {


struct Radian_tag {};
struct Degree_tag {};
struct Turn_tag {};


// Forward declaration to define the usings, so they are available in Angle definition
template <class T_representation, class T_unitTag = Radian_tag>
class Angle;


//
// Actual units for clients use
//
template <class T_representation>
using Radian = Angle<T_representation, Radian_tag>;

template <class T_representation>
using Degree = Angle<T_representation, Degree_tag>;

template <class T_representation>
using Turn = Angle<T_representation, Turn_tag>;


//
// Angle definition
//
template <class T_representation, class T_unitTag>
// TODO Ad 2023/10/25: Apply requirements on the representation type
//requires std::floating_point<T_representation> // Open question: is there any reason to support integral types? Or custom types?
class Angle
{
public:
    // Notably useful as the TT_angle template type in ::as()
    template <class T>
    using unit = Angle<T, T_unitTag>;

    constexpr Angle() noexcept= default;

    explicit constexpr Angle(T_representation aValue) noexcept :
        mValue{aValue}
    {}

    /*implicit*/ constexpr operator Angle<T_representation, Radian_tag>() const noexcept;

    template <template <class> class TT_angle>
    constexpr TT_angle<T_representation> as() const noexcept;

    constexpr T_representation value() const noexcept
    {
        return mValue;
    }

    T_representation & data() noexcept
    {
        return mValue;
    }

    constexpr Angle & operator+=(const Angle aRhs) noexcept;
    constexpr Angle & operator-=(const Angle aRhs) noexcept;
    template <class T_factor>
    constexpr Angle & operator*=(const T_factor aFactor) noexcept;
    template <class T_factor>
    constexpr Angle & operator/=(const T_factor aFactor);

    // Comparison operations should work for types implicitly convertible to Angle,
    // which would not be the case for a template free function defined outside the class.
    // see: https://stackoverflow.com/a/9789036/1027706
    constexpr friend bool operator==(const Angle aLhs, const Angle aRhs) noexcept
    {
        return aLhs.value() == aRhs.value();
    }

    constexpr friend bool operator!=(const Angle aLhs, const Angle aRhs) noexcept
    {
        return !(aLhs == aRhs);
    }

    constexpr friend bool operator<(const Angle aLhs, const Angle aRhs) noexcept
    {
        return aLhs.value() < aRhs.value();
    }

    constexpr friend bool operator>(const Angle aLhs, const Angle aRhs) noexcept
    {
        return aLhs.value() > aRhs.value();
    }

    constexpr friend bool operator<=(const Angle aLhs, const Angle aRhs) noexcept
    {
        return aLhs.value() <= aRhs.value();
    }

    constexpr friend bool operator>=(const Angle aLhs, const Angle aRhs) noexcept
    {
        return aLhs.value() >= aRhs.value();
    }

private:
    T_representation mValue{0};
};


//
// Operations
//
#define ANGLE Angle<T_representation, T_unitTag>
#define ANGLE_LEFT Angle<T_representation, T_leftUnitTag>
#define ANGLE_RIGHT Angle<T_representation, T_rightUnitTag>

template <class T_representation, class T_unitTag>
constexpr ANGLE abs(const ANGLE aAngle) noexcept
{
    return ANGLE{std::abs(aAngle.value())};
}

template <class T_representation, class T_unitTag>
T_representation sin(const ANGLE aAngle)
{
    return std::sin(Radian<T_representation>{aAngle}.value());
}

template <class T_representation, class T_unitTag>
T_representation cos(const ANGLE aAngle)
{
    return std::cos(Radian<T_representation>{aAngle}.value());
}

template <class T_representation, class T_unitTag>
T_representation tan(const ANGLE aAngle)
{
    return std::tan(Radian<T_representation>{aAngle}.value());
}

template <class T_unitTag, class T_representation>
ANGLE asin(const T_representation aSine)
{
    return ANGLE{std::asin(aSine)};
}

template <class T_unitTag, class T_representation>
ANGLE acos(const T_representation aCosine)
{
    return ANGLE{std::acos(aCosine)};
}

template <class T_unitTag, class T_representation>
ANGLE atan(const T_representation aTangent)
{
    return ANGLE{std::atan(aTangent)};
}

template <class T_representation, class T_unitTag>
constexpr ANGLE operator-(const ANGLE aAngle) noexcept
{
    return ANGLE{-aAngle.value()};
}

template <class T_representation, class T_unitTag>
constexpr ANGLE & ANGLE::operator+=(const Angle aRhs) noexcept
{
    mValue += aRhs.mValue;
    return *this;
}

template <class T_representation, class T_leftUnitTag, class T_rightUnitTag>
constexpr ANGLE_LEFT operator+(ANGLE_LEFT aLhs, const ANGLE_RIGHT aRhs) noexcept
{
    return aLhs += aRhs.template as<decltype(aLhs)::template unit>();
}

template <class T_representation, class T_unitTag>
constexpr ANGLE & ANGLE::operator-=(const Angle aRhs) noexcept
{
    mValue -= aRhs.mValue;
    return *this;
}

template <class T_representation, class T_leftUnitTag, class T_rightUnitTag>
constexpr ANGLE_LEFT operator-(ANGLE_LEFT aLhs, const ANGLE_RIGHT aRhs) noexcept
{
    return aLhs -= aRhs.template as<decltype(aLhs)::template unit>();
}

template <class T_representation, class T_unitTag>
template <class T_factor>
constexpr ANGLE & ANGLE::operator*=(const T_factor aFactor) noexcept
{
    mValue *= aFactor;
    return *this;
}

template <class T_representation, class T_unitTag, class T_factor>
constexpr ANGLE operator*(ANGLE aLhs, const T_factor aFactor) noexcept
{
    return aLhs *= aFactor;
}

template <class T_representation, class T_unitTag, class T_factor>
constexpr ANGLE operator*(const T_factor aFactor, ANGLE aRhs) noexcept
{
    return aRhs *= aFactor;
}

template <class T_representation, class T_unitTag>
template <class T_factor>
constexpr ANGLE & ANGLE::operator/=(const T_factor aFactor)
{
    mValue /= aFactor;
    return *this;
}

template <class T_representation, class T_unitTag, class T_factor>
constexpr ANGLE operator/(ANGLE aLhs, const T_factor aFactor)
{
    return aLhs /= aFactor;
}

template <class T_representation, class T_unitTag>
constexpr ANGLE reduce(ANGLE aLhs)
{
    constexpr T_representation halfRevolution =
        Radian<T_representation>{pi<T_representation>}.template as<ANGLE::template unit>().value();
    constexpr T_representation revolution = 2 * halfRevolution;
    // Note: 3D Math Primer (2nd) p. 241 listing 8.1 uses std::floor(angle + pi).
    // Yet, this would reduce to the range [-180, 180[ instead of ]-180, 180].
    return aLhs - ANGLE{std::ceil((aLhs.value() - halfRevolution) / revolution) * revolution};
}

#undef ANGLE_RIGHT
#undef ANGLE_LEFT
#undef ANGLE



//
// User-defined literals
//
namespace angle_literals
{

    inline Radian<double> operator"" _rad(long double aValue)
    { return Radian<double>(aValue); }

    inline Radian<float> operator"" _radf(long double aValue)
    { return Radian<float>(static_cast<float>(aValue)); }
    inline Radian<float> operator"" _radF(long double aValue)
    { return operator"" _radf(aValue); }

    inline Radian<long double> operator"" _radl(long double aValue)
    { return Radian<long double>(aValue); }
    inline Radian<long double> operator"" _radL(long double aValue)
    { return operator"" _radl(aValue); }

    inline Radian<int> operator"" _rad(unsigned long long aValue)
    { return Radian<int>(static_cast<int>(aValue)); }


    inline Degree<double> operator"" _deg(long double aValue)
    { return Degree<double>(aValue); }

    inline Degree<float> operator"" _degf(long double aValue)
    { return Degree<float>(static_cast<float>(aValue)); }
    inline Degree<float> operator"" _degF(long double aValue)
    { return operator"" _degf(aValue); }

    inline Degree<long double> operator"" _degl(long double aValue)
    { return Degree<long double>(aValue); }
    inline Degree<long double> operator"" _degL(long double aValue)
    { return operator"" _degl(aValue); }

    inline Degree<int> operator"" _deg(unsigned long long aValue)
    { return Degree<int>(static_cast<int>(aValue)); }

    inline Turn<double> operator"" _turn(long double aValue)
    { return Turn<double>(aValue); }

    inline Turn<float> operator"" _turnf(long double aValue)
    { return Turn<float>(static_cast<float>(aValue)); }
    inline Turn<float> operator"" _turnF(long double aValue)
    { return operator"" _turnf(aValue); }

    inline Turn<long double> operator"" _turnl(long double aValue)
    { return Turn<long double>(aValue); }
    inline Turn<long double> operator"" _turnL(long double aValue)
    { return operator"" _turnl(aValue); }

    inline Turn<int> operator"" _turn(unsigned long long aValue)
    { return Turn<int>(static_cast<int>(aValue)); }

} // namespace angle_literals


//
// Traits
//
template <class T_angle>
class Angle_trait;

template <class T_representation>
class Angle_trait<Radian<T_representation>>
{
public:
    static constexpr const char * const suffix = "rad";
    static constexpr double radian_factor = 1.0;
};

template <class T_representation>
class Angle_trait<Degree<T_representation>>
{
public:
    static constexpr const char * const suffix = "deg";
    // Would be nice to use T_representation here to match the floating point
    // yet that would break for integral types
    static constexpr double radian_factor = 2*pi<double>/360.0;
};

template <class T_representation>
class Angle_trait<Turn<T_representation>>
{
public:
    static constexpr const char * const suffix = "turn";
    // Would be nice to use T_representation here to match the floating point
    // yet that would break for integral types
    static constexpr double radian_factor = 2*pi<double>;
};


//
// IO
//
template <class T_representation, class T_unitTag>
std::ostream & operator<<(std::ostream & aOs, Angle<T_representation, T_unitTag> aAngle)
{
    return aOs << aAngle.value()
               << ' '
               << Angle_trait<Angle<T_representation, T_unitTag>>::suffix;
}


//
// Implementation
//
template <class T_representation, class T_unitTag>
constexpr Angle<T_representation, T_unitTag>::operator Angle<T_representation, Radian_tag> () const noexcept
{
    return this->as<Radian>();
}

template <class T_representation, class T_unitTag>
template <template <class> class TT_angle>
constexpr TT_angle<T_representation> Angle<T_representation, T_unitTag>::as() const noexcept
{
    // Note: allows narrowing
    return TT_angle<T_representation>(static_cast<T_representation>(
        value()
        * Angle_trait<Angle>::radian_factor
        / Angle_trait<TT_angle<T_representation>>::radian_factor
    ));
}

} // namespace math
} // namespace ad

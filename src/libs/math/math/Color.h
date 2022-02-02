#pragma once

#include "Vector.h"

#include <iomanip>


namespace ad {
namespace math {


#define ACCESSOR_DIMENSION(symbol, dimension)  \
    constexpr typename base_type::value_type & symbol()     \
    { return this->at(dimension-1); }                       \
    constexpr typename base_type::value_type symbol() const \
    { return this->at(dimension-1); }


template <class T_number>
struct RgbTrait
{
    static constexpr T_number max()
    {
        if constexpr(std::is_integral_v<T_number>)
        {
            return std::numeric_limits<T_number>::max();
        }
        else if constexpr(std::is_floating_point_v<T_number>)
        {
            return 1;
        }
    }
};
    

#define BASE Vector<Rgb_base<T_number>, 3, T_number>
template <class T_number>
class Rgb_base : public BASE
{
    /// \todo Disable most of the available functions
    typedef BASE base_type;
    // No constructor inheritance (see note on explicit constructor).

public:
    template<class T>
    using derived_type = Rgb_base<T>;

    /// \brief Constructor taking each channel value independently.
    /// \note This constructor is explicitly defined instead of relying
    /// on usual constructor inheritance, because otherwise the constexprness
    /// of literals is lost, causing "narrowing" errors on byte based colors instantiations.
    constexpr Rgb_base(T_number aRed, T_number aGreen, T_number aBlue) :
        base_type{aRed, aGreen, aBlue}
    {}

    ACCESSOR_DIMENSION(r, 1)
    ACCESSOR_DIMENSION(g, 2)
    ACCESSOR_DIMENSION(b, 3)
};
#undef BASE


#define BASE Vector<RgbAlpha_base<T_number>, 4, T_number>
template <class T_number>
class RgbAlpha_base : public BASE
{
    /// \todo Disable most of the available functions
    typedef BASE base_type;
    // No constructor inheritance (see note on explicit constructor).

public:
    template<class T>
    using derived_type = RgbAlpha_base<T>;

    /// \brief Constructor taking each channel value independently.
    /// \note This constructor is explicitly defined instead of relying
    /// on usual constructor inheritance, because otherwise the constexprness
    /// of literals is lost, causing "narrowing" errors on byte based colors instantiations.
    constexpr RgbAlpha_base(T_number aRed, T_number aGreen, T_number aBlue, T_number aOpacity = RgbTrait<T_number>::max()) :
        base_type{aRed, aGreen, aBlue, aOpacity}
    {}

    /*implicit*/ constexpr RgbAlpha_base(Rgb_base<T_number> aRgb, T_number aOpacity = RgbTrait<T_number>::max()) :
        base_type{aRgb.r(), aRgb.g(), aRgb.b(), aOpacity}
    {}

    ACCESSOR_DIMENSION(r, 1)
    ACCESSOR_DIMENSION(g, 2)
    ACCESSOR_DIMENSION(b, 3)
    ACCESSOR_DIMENSION(a, 4)
};
#undef BASE

namespace sdr {


    using Rgb = ::ad::math::Rgb_base<std::uint8_t>;

    constexpr const Rgb gBlack{0,   0,   0};
    constexpr const Rgb gWhite{255, 255, 255};

    constexpr const Rgb gRed  {255, 0,   0};
    constexpr const Rgb gGreen{0,   255, 0};
    constexpr const Rgb gBlue {0,   0,   255};

    constexpr const Rgb gYellow {gRed   + gGreen};
    constexpr const Rgb gCyan   {gGreen + gBlue};
    constexpr const Rgb gMagenta{gBlue  + gRed};

    using Rgba = ::ad::math::RgbAlpha_base<std::uint8_t>;
    // Please use implicit ctor from Rgb to get Rgba constants.
    constexpr const Rgba gTransparent{gBlack, 0};


    using Grayscale = std::uint8_t;


} // namespace sdr


namespace hdr {


    using Rgb = ::ad::math::Rgb_base<double>;


    constexpr const Rgb gBlack{0., 0., 0.};
    constexpr const Rgb gWhite{1., 1., 1.};

    constexpr const Rgb gRed  {1., 0., 0.};
    constexpr const Rgb gGreen{0., 1., 0.};
    constexpr const Rgb gBlue {0., 0., 1.};

    constexpr const Rgb gYellow {gRed   + gGreen};
    constexpr const Rgb gCyan   {gGreen + gBlue};
    constexpr const Rgb gMagenta{gBlue  + gRed};

    using Rgba = ::ad::math::RgbAlpha_base<double>;

    sdr::Rgb to_sdr(Rgb aHdr);
} // namespace hdr


} // namespace sdr


// CAUTION: For ADL, aRgb parameter is seen as the "aliased type", i.e. ::ad::math::Rgb_base
// in ::ad::math namespace, not in the alias sub-namespace
inline std::ostream & operator<<(std::ostream & os, const sdr::Rgb &aRgb)
{
    return os << "{"
        << std::setw(3) << static_cast<int>(aRgb.r()) << "; "
        << std::setw(3) << static_cast<int>(aRgb.g()) << "; "
        << std::setw(3) << static_cast<int>(aRgb.b()) << "}"
    ;
}


inline std::ostream & operator<<(std::ostream & os, const sdr::Rgba &aRgba)
{
    return os << "{"
        << std::setw(3) << static_cast<int>(aRgba.r()) << "; "
        << std::setw(3) << static_cast<int>(aRgba.g()) << "; "
        << std::setw(3) << static_cast<int>(aRgba.b()) << "; "
        << std::setw(3) << static_cast<int>(aRgba.a()) << "}"
    ;
}


inline sdr::Rgb to_sdr(hdr::Rgb aHdr)
{
    return {
        static_cast<sdr::Rgb::value_type>(std::min(1., aHdr.r()) * 255),
        static_cast<sdr::Rgb::value_type>(std::min(1., aHdr.g()) * 255),
        static_cast<sdr::Rgb::value_type>(std::min(1., aHdr.b()) * 255),
    };
}


#undef ACCESSOR_DIMENSION


} // namespace math
} // namespace ad

#pragma once

#include "Vector.h"

#include <iomanip>


namespace ad {
namespace math {


#define ACCESSOR_DIMENSION(symbol, dimension)  \
    typename base_type::value_type & symbol()                   \
    { return this->at(dimension-1); }       \
    typename base_type::value_type symbol() const               \
    { return this->at(dimension-1); }


#define BASE Vector<Rgb_base<T_number>, 3, T_number>
template <class T_number>
class Rgb_base : public BASE
{
    /// \todo Disable most of the available functions
    typedef BASE base_type;
    using base_type::base_type;

public:
    template<class T>
    using derived_type = Rgb_base<T>;

    ACCESSOR_DIMENSION(r, 1)
    ACCESSOR_DIMENSION(g, 2)
    ACCESSOR_DIMENSION(b, 3)
};
#undef BASE


namespace sdr {


using Rgb = ::ad::math::Rgb_base<std::uint8_t>;


constexpr const Rgb gBlack{(std::uint8_t)0,   (std::uint8_t)0,   (std::uint8_t)0};
constexpr const Rgb gWhite{(std::uint8_t)255, (std::uint8_t)255, (std::uint8_t)255};

constexpr const Rgb gRed  {(std::uint8_t)255, (std::uint8_t)0,   (std::uint8_t)0};
constexpr const Rgb gGreen{(std::uint8_t)0,   (std::uint8_t)255, (std::uint8_t)0};
constexpr const Rgb gBlue {(std::uint8_t)0,   (std::uint8_t)0,   (std::uint8_t)255};


} // namespace sdr


// CAUTION: For ADL, aRgb parameter is seen as the "aliased type", i.e. ::ad::math::Rgb_base
// in ::ad::math namespcace, not in the alias sub-namespace
inline std::ostream & operator<<(std::ostream & os, const sdr::Rgb &aRgb)
{
    return os << "{"
        << std::setw(3) << static_cast<int>(aRgb.r()) << "; "
        << std::setw(3) << static_cast<int>(aRgb.g()) << "; "
        << std::setw(3) << static_cast<int>(aRgb.b()) << "}";
}


#undef ACCESSOR_DIMENSION


} // namespace math
} // namespace ad

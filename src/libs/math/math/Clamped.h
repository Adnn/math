#pragma once


namespace ad {
namespace math {


/// \brief Clamps a value given on construction to the static interval [N_low, N_high].
template <class T_value = double,
          int N_low = 0,
          int N_high = 1
          // Requires C++20
          //auto F_low = []{ return (T_value)0; },
          //auto F_high = []{ return (T_value)1; }
          >
class Clamped
{
public:
    static constexpr T_value low_v = N_low;
    static constexpr T_value high_v = N_high;

    /*implicit*/ constexpr Clamped(const T_value & aValue);

    const T_value & value() const
    { return mValue; }

    /*implicit*/ operator const T_value & () const
    { return value(); }

private:
    T_value mValue;
};


//
// Implementations
//
template <class T_value, int N_low, int N_high>
constexpr Clamped<T_value, N_low, N_high>::Clamped(const T_value & aValue) :
    mValue{std::clamp(aValue, low_v, high_v)}
{}


} // namespace math
} // namespace ad

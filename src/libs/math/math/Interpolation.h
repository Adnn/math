#pragma once


#include <type_traits>


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


template <class T_value, int N_low, int N_high>
constexpr Clamped<T_value, N_low, N_high>::Clamped(const T_value & aValue) :
    mValue{std::clamp(aValue, low_v, high_v)}
{}


/// \brief Bound linear interpolation between aFirst and aLast.
template <class T_value, class T_parameter>
T_value lerp(const T_value & aFirst, const T_value & aLast, const Clamped<T_parameter> & aParameter)
{
    return aFirst + aParameter * (aLast - aFirst);
}


/// \brief Bound linear interpolation between aFirst and aLast.
template <class T_value, class T_parameter>
T_value lerp(const T_value & aFirst, const T_value & aLast, const T_parameter & aParameter)
{
    return aFirst + Clamped<T_parameter>{aParameter} * (aLast - aFirst);
}


/// \brief Unbound linear interpolation/extrapolation about aFirst and aLast.
template <class T_value, class T_parameter>
T_value lerpUnbound(const T_value & aFirst, const T_value & aLast, const T_parameter & aParameter)
{
    return aFirst + aParameter * (aLast - aFirst);
}


/// \brief Namespace containing all the easing functions.
namespace ease
{
    template <class T_parameter>
    struct Linear
    {
        static T_parameter ease(T_parameter aInput)
        { return aInput; }
    } ;

    /// \see https://en.wikipedia.org/wiki/Smoothstep
    template <class T_parameter>
    struct SmoothStep
    {
        static T_parameter ease(T_parameter aInput)
        {
            Clamped<T_parameter> x{aInput}; 
            return std::pow(x, 2) * (3 - 2 * x);
        }
    } ;
}



/// \brief Applies a static easing function to an input parameter, over a time duration.
///
/// It allows to apply an easing functions (quadratic, etc.) to a parameter, before
/// feeding it to a linerar interpolation function.
template <class T_parameter, class T_easeFunctor = ease::Linear<T_parameter>>
class ParameterAnimation
{
public:
    using Result_type = Clamped<T_parameter>;

    
    ParameterAnimation(T_parameter aDuration) :
        mSpeedFactor{(T_parameter)1 / aDuration}
    {}


    Result_type at(T_parameter aInput) const
    {
        return T_easeFunctor::ease(mSpeedFactor * aInput);
    }


    Result_type advance(T_parameter aIncrement)
    {
        return at(mAccumulatedInput += aIncrement);
    }


    bool isCompleted() const
    {
        return mAccumulatedInput * mSpeedFactor >= Result_type::high_v;
    }


private:
    T_parameter mSpeedFactor;
    T_parameter mAccumulatedInput{Result_type::low_v};
};


/// \brief Helper factory for ParameterAnimation.
template <template <class> class TT_easeFunctor, class T_parameter>
ParameterAnimation<T_parameter, TT_easeFunctor<T_parameter>> makeParameterAnimation(T_parameter aDuration)
{
    return {aDuration} ;
}


/// \brief Encapsulate a complete interpolation procedure, with bounds,
/// duration, and easing.
template <class T_value, class T_parameter, class T_easeFunctor>
class Interpolation
{
public:
    using Animation_type = ParameterAnimation<T_parameter, T_easeFunctor>;

    Interpolation(T_value aFirst, T_value aLast, Animation_type aAnimation) :
        mFirst{std::move(aFirst)},
        mLast{std::move(aLast)},
        mAnimation{std::move(aAnimation)}
    {}


    T_value advance(T_parameter aIncrement)
    {
        return lerp(mFirst, mLast, mAnimation.advance(aIncrement));
    }


    T_value at(T_parameter aInput) const
    {
        return lerp(mFirst, mLast, mAnimation.at(aInput));
    }


    bool isCompleted() const
    {
        return mAnimation.isCompleted();
    }


private:
    T_value mFirst;
    T_value mLast;
    Animation_type mAnimation;
};


/// \brief Helper factory for Interpolation.
template <template <class> class TT_easeFunctor, class T_value, class T_parameter>
Interpolation<T_value, T_parameter, TT_easeFunctor<T_parameter>>
makeInterpolation(const T_value & aFirst, const T_value & aLast, T_parameter aDuration)
{
    static_assert(! std::is_integral_v<T_parameter>,
                  "Using an integral parameter to interpolate between [0..1] is very likely an error.");
    return Interpolation(
        aFirst,
        aLast,
        makeParameterAnimation<TT_easeFunctor>(aDuration));
}


} // namespace math
} // namespace ad

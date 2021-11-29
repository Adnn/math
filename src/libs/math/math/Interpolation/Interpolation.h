#pragma once


#include "ParameterAnimation.h"
#include "../Clamped.h"

#include <type_traits>


namespace ad {
namespace math {


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


/// \brief Encapsulate a complete interpolation procedure, with bounds,
/// duration, and easing.
template <class T_value, class T_parameter,
          template <class> class TT_periodicity, template <class> class TT_easeFunctor>
class Interpolation
{
public:
    using Animation_type = ParameterAnimation<T_parameter, Clamp, TT_periodicity, TT_easeFunctor>;

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
template <template <class> class TT_periodicity = None,
          template <class> class TT_easeFunctor = None,
          class T_value,
          class T_parameter>
Interpolation<T_value, T_parameter, TT_periodicity, TT_easeFunctor>
makeInterpolation(const T_value & aFirst, const T_value & aLast, T_parameter aPeriod)
{
    static_assert(! std::is_integral_v<T_parameter>,
                  "Using an integral parameter to interpolate between [0..1] is very likely an error.");
    return Interpolation{
        aFirst,
        aLast,
        makeParameterAnimation<Clamp, TT_periodicity, TT_easeFunctor>(aPeriod)
    };
}


} // namespace math
} // namespace ad

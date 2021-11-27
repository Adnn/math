#pragma once


#include "../Clamped.h"


namespace ad {
namespace math {


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


    explicit ParameterAnimation(T_parameter aDuration) :
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
    return ParameterAnimation<T_parameter, TT_easeFunctor<T_parameter>>{aDuration};
}


} // namespace math
} // namespace ad

#pragma once


#include "../Clamped.h"


namespace ad {
namespace math {


/// \brief Namespace containing all the easing functions.
namespace ease
{
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
} // namespae ease


/// \brief Namespace containing all the peridicity functions.
namespace periodic
{
    template <class T_parameter>
    struct Repeat
    {
        // TODO give a name?
        T_parameter operator()(T_parameter aPeriod, T_parameter aAbsoluteValue) const
        {
            return aAbsoluteValue - std::floor(aAbsoluteValue/aPeriod) * aPeriod;
        }
    };

    template <class T_parameter>
    struct PingPong
    {
        T_parameter operator()(T_parameter aPeriod, T_parameter aAbsoluteValue) const
        {
            T_parameter doubleDuration = 2 * aPeriod;
            T_parameter doublePeriod =
                aAbsoluteValue - std::floor(aAbsoluteValue/doubleDuration) * doubleDuration;

            return doublePeriod - 2 * std::max(T_parameter{0}, doublePeriod - aPeriod);
        }
    };
} // namespace periodicity


/// \brief Enumaration to control wether ParameterAnimation output should be clamped.
enum AnimationResult
{
    FullRange = 0,
    Clamp,
};


/// \brief An empty class template, notably used to disable easing / periodicity.
template <class>
struct None
{};


namespace detail 
{
    template <class T_parameter, AnimationResult N_resultRange>
    class ParameterAnimation_ResultHelper
    {
    public:
        using Result_type = T_parameter;

    protected:
        explicit ParameterAnimation_ResultHelper(T_parameter aSpeed) :
            mSpeed{std::move(aSpeed)}
        {}

        T_parameter mSpeed;
    };


    // Specialization when the result is to be clamped.
    template <class T_parameter>
    class ParameterAnimation_ResultHelper<T_parameter, Clamp>
    {
    public:
        using Result_type = Clamped<T_parameter>;

    protected:
        static constexpr T_parameter mSpeed{1};
    };
} // namespace detail


// TODO potential optimization (only speed or period member) for no-easing/no-periodicity
/// \brief Animate a 1D parameter value, with speed factor, and optional easing and periodicity.
/// The output might be clamped to [0 1] (notably useful for lerp()), or full range.
/// 
/// This is intended to cover most realistic cases, thanks to the different comibinations availables.
template <class T_parameter,
          AnimationResult N_resultRange,
          template <class> class TT_periodicity = None,
          template <class> class TT_easeFunctor = None>
class ParameterAnimation : public detail::ParameterAnimation_ResultHelper<T_parameter, N_resultRange>
{
    using Base_t = detail::ParameterAnimation_ResultHelper<T_parameter, N_resultRange>;
    using typename Base_t::Result_type;
    using Base_t::mSpeed;

    // Note: Clang wants these to be "instantiated" before the use in HasSpeed
    static constexpr bool IsEasing()
    { return !std::is_same_v<TT_easeFunctor<void>, None<void>>; }

    static constexpr bool IsPeriodic()
    { return !std::is_same_v<TT_periodicity<void>, None<void>>; }

    static constexpr bool IsClamped()
    { return N_resultRange == Clamp; }

    /// \brief The trivial case does not have a notion of period, only speed.
    static constexpr bool IsTrivial()
    { return !IsClamped() && !IsPeriodic() && !IsEasing(); }

public:
    // Note: must be available before its use in ctors.
    static constexpr bool HasSpeed()
    {
        return !IsClamped();
    }

    /// \brief Constructor from a period (i.e. duration). Available for clamped output.
    template <bool N_hasSpeed = HasSpeed()>
    explicit ParameterAnimation(T_parameter aPeriod, std::enable_if_t<!N_hasSpeed>* = nullptr) :
        mPeriod{std::move(aPeriod)}
    {}

    /// \brief Constructor from a period (i.e. duration) plus an optional speed. Available for full range output.
    template <bool N_hasSpeed = HasSpeed()>
    explicit ParameterAnimation(T_parameter aPeriod, T_parameter aSpeed = T_parameter{1},
                                std::enable_if_t<N_hasSpeed>* = nullptr) :
        Base_t{std::move(aSpeed)},
        mPeriod{std::move(aPeriod)}
    {}


    Result_type at(T_parameter aInput) const
    {
        // Note: This is trying its best to implement the overall animation logic below:
        //   amplitude * easeFunctor(normalize(periodicBehaviour(speedFactor * aInput)));
        // while statically disabling what is not relevant in the specific template instantiation.

        aInput *= mSpeed;

        if constexpr(IsPeriodic())
        {
            aInput = mPeriodicBehaviour(mPeriod, aInput);
        }

        if constexpr(IsEasing())
        {
            static_assert(!std::is_integral_v<T_parameter>,
                          "Integral parameters require more work regarding normalization.");

            // Need to normalize the easing input
            aInput = TT_easeFunctor<T_parameter>::ease(aInput / mPeriod);

            if constexpr(N_resultRange == FullRange)
            {
                // Then expand it back to the full amplitude if result is not full range
                aInput *= mPeriod;
            }
        }
        else if constexpr(IsClamped())
        {
            // Normalize the input despite absence of easing, as the output should be clamped.
            // This is equivalent to multiplying by a speed factor that would be 1/period.
            aInput /= mPeriod;
        }

        return aInput;
    }


    Result_type advance(T_parameter aIncrement)
    {
        return at(mAccumulatedInput += aIncrement);
    }

    /// \brief Indicates if this animation can reach completion (or if it goes on forever).
    static constexpr bool IsFinite()
    {
        // The periodic case goes on forever by definition.
        // The trivial case is also infinite, as the output will lineraly grows.
        return !IsPeriodic() && !IsTrivial();
    }


    /// \brief Indicate whether this animation is done.
    /// 
    /// When it is done, it is guaranteed that advancing by non-negative values
    /// will keep on producing the same parameter output.
    bool isCompleted() const
    {
        if constexpr(IsFinite())
        {
            return (mAccumulatedInput * mSpeed) >= mPeriod;
        }
        else
        {
            return false;
        }
    }


private:
    T_parameter mPeriod;
    T_parameter mAccumulatedInput{0};
    TT_periodicity<T_parameter> mPeriodicBehaviour; // empty class for basic initial cases (Repeat, PingPong)
                                                    // but leaves room for more potential other scenarios.

    // Would be better to have the asseration at the beginning, but the static member functions must be defined.
    static_assert(!IsTrivial(), "Please use TrivialParameterAnimation for the trivial case.");
};


/// \brief Partial specialization for the **trivial** case of a full-range non-easing non-periodic parameter animation.
///
/// It amounts to multiplying the accumulated input by the speed.
template <class T_parameter>
class ParameterAnimation<T_parameter, FullRange, None, None> :
    public detail::ParameterAnimation_ResultHelper<T_parameter, FullRange>
{
    using Base_t = detail::ParameterAnimation_ResultHelper<T_parameter, FullRange>;
    using Base_t::mSpeed;

public:
    explicit ParameterAnimation(T_parameter aSpeed) :
        Base_t{std::move(aSpeed)}
    {}

    typename Base_t::Result_type at(T_parameter aInput) const
    {
        return aInput * mSpeed;
    }

    typename Base_t::Result_type advance(T_parameter aIncrement)
    {
        return at(mAccumulatedInput += aIncrement);
    }

    static constexpr bool HasSpeed()
    { return true; }

    static constexpr bool IsFinite()
    { return false; }

    bool isCompleted() const
    { return false; }

private:
    T_parameter mAccumulatedInput{0};
};


/// \brief Helper factory for ParameterAnimation, taking a period.
template <AnimationResult N_resultRange,
          template <class> class TT_periodicity = None,
          template <class> class TT_easeFunctor = None,
          class T_parameter>
ParameterAnimation<T_parameter, N_resultRange, TT_periodicity, TT_easeFunctor> makeParameterAnimation(T_parameter aSingleParameter)
{
    return ParameterAnimation<T_parameter, N_resultRange, TT_periodicity, TT_easeFunctor>{aSingleParameter};
}


#define PARAMETER_ANIMATION ParameterAnimation<T_parameter, N_resultRange, TT_periodicity, TT_easeFunctor>

/// \brief Helper factory for ParameterAnimation, taking both period and speed.
///
/// It is only available when the constructed ParameterAnimation has a speed.
template <AnimationResult N_resultRange,
          template <class> class TT_periodicity = None,
          template <class> class TT_easeFunctor = None,
          class T_parameter>
std::enable_if_t<PARAMETER_ANIMATION::HasSpeed(), PARAMETER_ANIMATION> 
makeParameterAnimation(T_parameter aPeriod, T_parameter aSpeed)
{
    return ParameterAnimation<T_parameter, N_resultRange, TT_periodicity, TT_easeFunctor>{aPeriod, aSpeed};
}

#undef PARAMETER_ANIMATION

} // namespace math
} // namespace ad

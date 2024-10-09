#pragma once

#include "reflexion/NameValuePair.h"
#include "../Clamped.h"
#include "../Vector.h"
#include "../Constants.h"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <vector>

namespace ad {
namespace math {

template <class T_number>
T_number cubeRoot(const T_number & a)
{
    return std::copysign(std::pow(std::abs(a), 1 / 3.f), a);
}


/// \brief Namespace containing all the easing functions.
namespace ease
{
template <class T_parameter>
struct Bezier
{
    // One start point with a handle 18 points with two handles and an end
    // point with one handle
    constexpr static size_t sMaxStageSize = 2 + 18 * 3 + 2;

    // NFFNFFNFFNFFN
    // indices for on curve are 0, 3, 6, 9, 12
    std::array<T_parameter, Bezier::sMaxStageSize> mXValues;
    std::array<T_parameter, Bezier::sMaxStageSize> mYValues;
    size_t mOnCurveCount;

    Bezier() :
        mXValues{0.f, 0.f, 1.f, 1.f},
        mYValues{0.f, 0.f, 1.f, 1.f},
        mOnCurveCount{2}
    {}

    T_parameter ease(T_parameter aInput) const
    {
        size_t valueIndex = getValueIndex(aInput) * 3;
        return easeFromIndex(aInput, valueIndex);
    }

    T_parameter easeFromIndex(T_parameter aInput, size_t aValueIndex) const
    {
        T_parameter startOnCurve = mYValues.at(aValueIndex);
        T_parameter startOffCurve = mYValues.at(aValueIndex + 1);
        T_parameter endOffCurve = mYValues.at(aValueIndex + 2);
        T_parameter endOnCurve = mYValues.at(aValueIndex + 3);

        T_parameter startOnCurveX = mXValues.at(aValueIndex);
        T_parameter startOffCurveX = mXValues.at(aValueIndex + 1);
        T_parameter endOffCurveX = mXValues.at(aValueIndex + 2);
        T_parameter endOnCurveX = mXValues.at(aValueIndex + 3);
        T_parameter t =
            getCubicRoots(startOnCurveX - aInput, startOffCurveX - aInput,
                          endOffCurveX - aInput, endOnCurveX - aInput)
                .at(0);

        T_parameter tSqr = t * t;
        T_parameter OneMinusT = 1 - t;
        T_parameter OneMinusTSqr = (1 - t) * (1 - t);

        return startOnCurve * OneMinusTSqr * OneMinusT
               + startOffCurve * 3 * OneMinusTSqr * t
               + endOffCurve * 3 * OneMinusT * tSqr + endOnCurve * t * tSqr;
    }

    size_t getValueIndex(T_parameter aInput) const
    {
        size_t currentIndex = 0;
        while (currentIndex < mOnCurveCount - 1
               && mXValues.at((currentIndex + 1) * 3) < aInput)
        {
            currentIndex++;
        }
        return currentIndex;
    }

    int addPoint(T_parameter aInput)
    {
        if (mOnCurveCount < 20)
        {
            size_t easingIndex = getValueIndex(aInput);
            T_parameter yValue = easeFromIndex(aInput, easingIndex * 3);
            size_t insertIndex = (easingIndex + 1) * 3 - 1;
            mOnCurveCount++;
            for (int i = mOnCurveCount * 3 - 2; i >= insertIndex + 3; i--)
            {
                mXValues.at(i) = mXValues.at(i - 3);
                mYValues.at(i) = mYValues.at(i - 3);
            }
            mXValues.at(insertIndex) = aInput;
            mXValues.at(insertIndex + 1) = aInput;
            mXValues.at(insertIndex + 2) = aInput;
            mYValues.at(insertIndex) = yValue;
            mYValues.at(insertIndex + 1) = yValue;
            mYValues.at(insertIndex + 2) = yValue;

            return insertIndex + 1;
        }

        return -1;
    }

    void removePoint(size_t aIndex)
    {
        if (mOnCurveCount > 2)
        {
            for (int i = aIndex - 1; i < mOnCurveCount * 3 - 2; i++)
            {
                mXValues.at(i) = mXValues.at(i + 3);
                mYValues.at(i) = mYValues.at(i + 3);
            }
            mOnCurveCount--;
        }
    }

    void changePoint(size_t index, math::Position<2, T_parameter> aPosition)
    {
        if (index == 0 || index == (mOnCurveCount * 3) - 2 - 1)
        {
            return;
        }

        switch(index % 3)
        {
            case 0:
            {
                float oldX = mXValues.at(index);
                float oldY = mYValues.at(index);
                if (index > 0)
                {
                    mXValues.at(index - 1) = std::clamp((mXValues.at(index - 1) - oldX) + aPosition.x(), T_parameter{0}, T_parameter{1});
                    mYValues.at(index - 1) = std::clamp((mYValues.at(index - 1) - oldY) + aPosition.y(), T_parameter{0}, T_parameter{1});
                }

                if (index < mXValues.size() - 1)
                {
                    mXValues.at(index + 1) = std::clamp((mXValues.at(index + 1) - oldX) + aPosition.x(), T_parameter{0}, T_parameter{1});
                    mYValues.at(index + 1) = std::clamp((mYValues.at(index + 1) - oldY) + aPosition.y(), T_parameter{0}, T_parameter{1});
                }
                break;
            }
            case 1:
            {
                float onX = mXValues.at(index - 1);
                aPosition.x() = std::max(onX, aPosition.x());
                break;
            }
            case 2:
            {
                float onX = mXValues.at(index + 1);
                aPosition.x() = std::min(onX, aPosition.x());
                break;
            }
        }
        mXValues.at(index) = std::clamp(aPosition.x(), T_parameter{0}, T_parameter{1});
        mYValues.at(index) = std::clamp(aPosition.y(), T_parameter{0}, T_parameter{1});
    }

    std::vector<math::Position<2, float>> getKnots() const
    {
        std::vector<math::Position<2, float>> ret;

        for (int i = 0; i < mOnCurveCount - 1; i++)
        {
            ret.push_back({mXValues.at(i * 3), mYValues.at(i * 3)});
            ret.push_back({mXValues.at(i * 3 + 1), mYValues.at(i * 3 + 1)});
            ret.push_back({mXValues.at(i * 3 + 2), mYValues.at(i * 3 + 2)});
        }

        ret.push_back({mXValues.at((mOnCurveCount - 1) * 3), mYValues.at((mOnCurveCount - 1) * 3)});

        return ret;
    }

    std::array<T_parameter, 3> getCubicRoots(T_parameter pa,
                                             T_parameter pb,
                                             T_parameter pc,
                                             T_parameter pd) const
    {
        T_parameter a = -pa + (3.f * pb) - (3.f * pc) + pd;
        T_parameter b = (3.f * pa) - (6.f * pb) + (3.f * pc);
        T_parameter c = (-3.f * pa) + (3.f * pb);
        T_parameter d = pa;

        if (epsilonCompare(a, 0.f))
        {
            if (epsilonCompare(b, 0.f))
            {
                if (epsilonCompare(c, 0.f))
                {
                    return {};
                }
                return {-c / b};
            }
            T_parameter q = std::sqrt((b * b) - (4.f * a * c));
            T_parameter twoA = 2.f * a;
            return {(-b + q) / twoA, (-b - q) / twoA};
        }

        b /= a;
        c /= a;
        d /= a;

        T_parameter p = ((3.f * c) - (b * b)) / 3.f;
        T_parameter p3 = p / 3.f;
        T_parameter q =
            ((2.f * b * b * b) - (9.f * b * c) + (27.f * d)) / 27.f;
        T_parameter q2 = q / 2.f;
        T_parameter discrimant = (q2 * q2) + (p3 * p3 * p3);
        T_parameter root;
        T_parameter u;
        T_parameter v;
        int index = 0;
        std::array<T_parameter, 3> result;

        // We need to do everything because sometimes the polynomial
        // has multiple root but only one in 0 -- 1
        if (discrimant < 0)
        {
            T_parameter mp3 = -p3;
            T_parameter mp33 = mp3 * mp3 * mp3;
            T_parameter r = std::sqrt(mp33);
            T_parameter t = -q2 / r;
            T_parameter cosphi = t < -1.f ? -1.f : t > 1.f ? 1. : t;
            T_parameter phi = std::acos(cosphi);
            T_parameter t1 = cubeRoot(r) * 2.f;

            root = (t1 * std::cos(phi / 3.f)) - (b / 3.f);
            if (root >= 0.f && root <= 1.f)
            {
                result.at(index++) = root;
            }
            root = (t1 * std::cos((phi + 2.f * pi<float>) / 3.f)) - (b / 3.f);
            if (root >= 0.f && root <= 1.f)
            {
                result.at(index++) = root;
            }
            root = (t1 * std::cos((phi + 4.f * pi<float>) / 3.f)) - (b / 3.f);
            if (root >= 0.f && root <= 1.f)
            {
                result.at(index++) = root;
            }
            return result;
        }

        if (discrimant == 0)
        {
            u = q2 < 0 ? cubeRoot(-q2) : -cubeRoot(q2);
            root = 2.f * u - b / 3.f;
            if (root >= 0.f && root <= 1.f)
            {
                result.at(index++) = root;
            }
            root = -u - b / 3.f;
            if (root >= 0.f && root <= 1.f)
            {
                result.at(index++) = root;
            }
        }

        T_parameter sd = std::sqrt(discrimant);
        u = cubeRoot(-q2 + sd);
        v = cubeRoot(q2 + sd);
        root = u - v - b / 3.f;

        if (root >= 0.f && root <= 1.f)
        {
            result.at(index++) = root;
        }

        return result;
    }
};
/// \see https://en.wikipedia.org/wiki/Smoothstep
template <class T_parameter>
struct SmoothStep
{
    T_parameter ease(T_parameter aInput) const
    {
        Clamped<T_parameter> x{aInput};
        return std::pow(x, 2) * (3 - 2 * x);
    }

    std::vector<math::Position<2, float>> getKnots() const
    {
        return {{0.f, 0.f}, {1.f, 1.f}};
    }
};

template <class T_parameter>
struct CubicSpline
{
    CubicSpline(std::vector<T_parameter> && aXValues,
                std::vector<T_parameter> && aYValues,
                T_parameter aX0Prime = T_parameter{0},
                T_parameter aXNPrime = T_parameter{0}) :
        xValues{std::move(aXValues)},
        yValues{std::move(aYValues)},
        x0Prime{aX0Prime},
        xNPrime{aXNPrime}
    {
        computeSecDerivative();
    }

    CubicSpline(std::vector<Position<2, float>> aValues,
                T_parameter aX0Prime = T_parameter{0},
                T_parameter aXNPrime = T_parameter{0}) :
        x0Prime{aX0Prime}, xNPrime{aXNPrime}
    {
        xValues.reserve(aValues.size());
        yValues.reserve(aValues.size());

        for (auto value : aValues)
        {
            xValues.emplace_back(value.x());
            yValues.emplace_back(value.y());
        }
        computeSecDerivative();
    }

    std::vector<math::Position<2, float>> getKnots() const
    {
        std::vector<math::Position<2, float>> ret;

        for (int i = 0; i < xValues.size(); i++)
        {
            ret.push_back({xValues.at(i), yValues.at(i)});
        }

        return ret;
    }

    void computeSecDerivative()
    {
        assert(xValues.size() == yValues.size());
        assert(xValues.size() > 2);

        size_t valLength = xValues.size();
        for (int i = 0; i < valLength - 1; i++)
        {
            assert(xValues.at(i) < xValues.at(i + 1));
        }

        std::vector<T_parameter> cPrime(valLength, 0);
        std::vector<T_parameter> yPrimePrime(valLength, 0);

        T_parameter newX = xValues.at(1);
        T_parameter newY = yValues.at(1);
        T_parameter cj = xValues.at(1) - xValues.at(0);
        T_parameter newDj = (yValues.at(1) - yValues.at(0)) / cj;

        if (x0Prime == T_parameter{0})
        {
            cPrime.at(0) = T_parameter{0};
            yPrimePrime.at(0) = T_parameter{0};
        }
        else
        {
            cPrime.at(0) = T_parameter{0.5};
            yPrimePrime.at(0) = T_parameter{3 * (newDj - x0Prime) / cj};
        }

        for (int i = 1; i < valLength - 1; i++)
        {
            T_parameter oldX = newX;
            T_parameter oldY = newY;

            T_parameter aj = cj;
            T_parameter oldDj = newDj;

            newX = xValues.at(i + 1);
            newY = yValues.at(i + 1);

            cj = newX - oldX;
            newDj = (newY - oldY) / cj;
            T_parameter bj = T_parameter{2} * (cj + aj);
            T_parameter invDenom =
                T_parameter{1} / (bj - (aj * cPrime.at(i - 1)));
            T_parameter dj = T_parameter{6} * (newDj - oldDj);

            yPrimePrime.at(i) =
                (dj - aj * yPrimePrime.at(i - 1)) * invDenom;
            cPrime.at(i) = cj * invDenom;
        }

        size_t lastElementIndex = valLength - 1;
        if (xNPrime == T_parameter{0})
        {
            cPrime.at(lastElementIndex) = 0;
            yPrimePrime.at(lastElementIndex) = 0;
        }
        else
        {
            T_parameter aj = cj;
            T_parameter oldDj = newDj;

            cj = 0;
            newDj = xNPrime;
            T_parameter bj = T_parameter{2} * (cj + aj);
            T_parameter invDenom =
                T_parameter{1}
                / (bj - aj * cPrime.at(lastElementIndex - 1));
            T_parameter dj = T_parameter{6} * (newDj - oldDj);

            yPrimePrime.at(lastElementIndex) =
                (dj - aj * yPrimePrime.at(lastElementIndex - 1)) * invDenom;
            cPrime.at(lastElementIndex) = cj * invDenom;
        }

        for (int i = (int) valLength - 2; i >= 0; i--)
        {
            yPrimePrime.at(i) =
                yPrimePrime.at(i) - cPrime.at(i) * yPrimePrime.at(i + 1);
        }

        fppValues = yPrimePrime;
    }

    T_parameter ease(T_parameter aInput) const
    {
        size_t valueIndex = getValueIndex(aInput);
        return interpSpline(
            aInput, xValues.at(valueIndex), xValues.at(valueIndex + 1),
            yValues.at(valueIndex), yValues.at(valueIndex + 1),
            fppValues.at(valueIndex), fppValues.at(valueIndex + 1));
    }

    static constexpr T_parameter oneSixth = T_parameter{1} / T_parameter{6};

    size_t getValueIndex(T_parameter aInput) const
    {
        size_t index = 0;

        while (index + 1 < xValues.size() && xValues.at(index + 1) < aInput)
        {
            index++;
        }

        return index;
    }

    int addPoint(T_parameter aValue) {return -1;}

    void changePoint(size_t index, math::Position<2, T_parameter> aPoint) {}

    std::vector<T_parameter> xValues;
    std::vector<T_parameter> yValues;
    // double derivatives values
    std::vector<T_parameter> fppValues;
    T_parameter x0Prime;
    T_parameter xNPrime;
};
} // namespace ease

/// \brief Namespace containing all the peridicity functions.
namespace periodic {
template <class T_parameter>
struct Repeat
{
    // TODO give a name?
    T_parameter operator()(T_parameter aPeriod,
                           T_parameter aAbsoluteValue) const
    {
        return aAbsoluteValue - std::floor(aAbsoluteValue / aPeriod) * aPeriod;
    }
};

template <class T_parameter>
struct PingPong
{
    T_parameter operator()(T_parameter aPeriod,
                           T_parameter aAbsoluteValue) const
    {
        T_parameter doubleDuration = 2 * aPeriod;
        T_parameter doublePeriod =
            aAbsoluteValue
            - std::floor(aAbsoluteValue / doubleDuration) * doubleDuration;

        return doublePeriod
               - 2 * std::max(T_parameter{0}, doublePeriod - aPeriod);
    }
};
} // namespace periodic

/// \brief Enumaration to control wether ParameterAnimation output should be
/// clamped.
enum AnimationResult
{
    FullRange = 0,
    Clamp,
};

/// \brief An empty class template, notably used to disable easing /
/// periodicity.
template <class>
struct None
{};

namespace detail {
template <class T_parameter, AnimationResult N_resultRange>
class ParameterAnimation_ResultHelper
{
public:
    using Result_type = T_parameter;

protected:
    explicit ParameterAnimation_ResultHelper(T_parameter aSpeed) :
        mSpeed{std::move(aSpeed)}
    {}

    // Note: it is intended to remain constant, because the "accumulated" input
    // is multiplied by speed (so a change of speed would be "retro-active")
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

// TODO Implement a "dynamic" version (non-templated, having data members to
// control behaviour)

// TODO Find a more descriptive type name.
/// \brief Animate a 1D parameter value, with speed factor, and optional easing
/// and periodicity. The output might be clamped to [0 1] (notably useful for
/// lerp()), or full range.
///
/// This is intended to cover most realistic cases, thanks to the different
/// available combinations.
template <class T_parameter,
          AnimationResult N_resultRange,
          template <class> class TT_periodicity = None,
          template <class> class TT_easeFunctor = None>
class ParameterAnimation
    : public detail::ParameterAnimation_ResultHelper<T_parameter, N_resultRange>
{
    using Base_t =
        detail::ParameterAnimation_ResultHelper<T_parameter, N_resultRange>;
    using Base_t::mSpeed;
    using typename Base_t::Result_type;

    // Note: Clang wants these to be "instantiated" before the use in HasSpeed
    static constexpr bool IsEasing()
    {
        return !std::is_same_v<TT_easeFunctor<void>, None<void>>;
    }

    static constexpr bool IsPeriodic()
    {
        return !std::is_same_v<TT_periodicity<void>, None<void>>;
    }

    template <bool N_isEasing = IsEasing()>
    static constexpr bool IsModifiable()
    {
        return requires(const T_parameter & t) { t.addPoint(); };
    }

    static constexpr bool IsClamped() { return N_resultRange == Clamp; }

    /// \brief The trivial case does not have a notion of period, only speed.
    static constexpr bool IsTrivial()
    {
        return !IsClamped() && !IsPeriodic() && !IsEasing();
    }

public:
    // Note: must be available before its use in ctors.
    static constexpr bool HasSpeed() { return !IsClamped(); }

    /// \brief Constructor from a period (i.e. duration). Available for clamped
    /// output.
    template <bool N_hasSpeed = HasSpeed()>
    explicit ParameterAnimation(T_parameter aPeriod,
                                std::enable_if_t<!N_hasSpeed> * = nullptr) :
        mPeriod{std::move(aPeriod)}
    {}

    /// \brief Constructor from a period (i.e. duration) plus an optional speed.
    /// Available for full range output.
    template <bool N_hasSpeed = HasSpeed()>
    explicit ParameterAnimation(T_parameter aPeriod,
                                T_parameter aSpeed = T_parameter{1},
                                std::enable_if_t<N_hasSpeed> * = nullptr) :
        Base_t{std::move(aSpeed)}, mPeriod{std::move(aPeriod)}
    {}

    template <bool N_hasSpeed = HasSpeed()>
    explicit ParameterAnimation(TT_easeFunctor<T_parameter> aEaseFunctor,
                                T_parameter aPeriod = T_parameter{1},
                                T_parameter aSpeed = T_parameter{1},
                                std::enable_if_t<N_hasSpeed> * = nullptr) :
        Base_t{std::move(aSpeed)},
        mPeriod{std::move(aPeriod)},
        mEaseFunctor{aEaseFunctor}
    {}

    template <bool N_isEasing = IsEasing()>
    std::vector<math::Position<2, float>>
    getKnots(std::enable_if_t<N_isEasing> * = nullptr)
    {
        return mEaseFunctor.getKnots();
    }

    Result_type at(T_parameter aInput) const
    {
        // Note: This is trying its best to implement the overall animation
        // logic below:
        //   amplitude * easeFunctor(normalize(periodicBehaviour(speedFactor *
        //   aInput)));
        // while statically disabling what is not relevant in the specific
        // template instantiation.

        aInput *= mSpeed;

        if constexpr (IsPeriodic())
        {
            aInput = mPeriodicBehaviour(mPeriod, aInput);
        }

        if constexpr (IsEasing())
        {
            static_assert(!std::is_integral_v<T_parameter>,
                          "Integral parameters require more work regarding "
                          "normalization.");

            // Need to normalize the easing input
            aInput = mEaseFunctor.ease(aInput / mPeriod);

            if constexpr (N_resultRange == FullRange)
            {
                // Then expand it back to the full amplitude if result is not
                // full range
                aInput *= mPeriod;
            }
        }
        else if constexpr (IsClamped())
        {
            // Normalize the input despite absence of easing, as the output
            // should be clamped. This is equivalent to multiplying by a speed
            // factor that would be 1/period.
            aInput /= mPeriod;
        }

        return aInput;
    }

    /// \brief Indicates if this animation can reach completion (or if it goes
    /// on forever).
    static constexpr bool IsFinite()
    {
        // The periodic case goes on forever by definition.
        // The trivial case is also infinite, as the output will lineraly grows.
        return !IsPeriodic() && !IsTrivial();
    }

    /// \brief Indicate whether the animation is done at `aInput`.
    /// \note If this is **not** finite, the call will always return `false`.
    ///
    /// When it is done, it is guaranteed that any input >= `aInput`
    /// will produce the same parameter output, which is the final value.
    bool isCompleted(T_parameter aInput) const
    {
        if constexpr (IsFinite())
        {
            return (aInput * mSpeed) >= mPeriod;
        }
        else
        {
            return false;
        }
    }

    /// \brief Returns how much excess advance is left after completion.
    /// \note If this is **not** finite, the call will always return zero.
    ///
    /// This is usefull to chain parameter animations, to carry-over the
    /// overshoot.
    T_parameter getOvershoot(T_parameter aInput) const
    {
        if constexpr (IsFinite())
        {
            return std::max<T_parameter>(0, aInput - (mPeriod / mSpeed));
        }
        else
        {
            return T_parameter{0};
        }
    }

    template <bool N_hasModification = IsModifiable()>
    int addPoint(T_parameter aValue)
    {
        return mEaseFunctor.addPoint(aValue);
    }

    template <bool N_hasModification = IsModifiable()>
    void removePoint(size_t aIndex)
    {
        mEaseFunctor.removePoint(aIndex);
    }

    template <bool N_hasModification = IsModifiable()>
    void changePoint(size_t index, math::Position<2, T_parameter> aPoint)
    {
        mEaseFunctor.changePoint(index, aPoint);
    }
    
    template<class T_witness>
    void describeTo(T_witness && w)
    {
        w.witness(NVP(mPeriod));
        if constexpr (!std::is_same_v<TT_periodicity<T_parameter>, None<T_parameter>>)
        {
            w.witness(NVP(mPeriodicBehaviour));
        }
        if constexpr (!std::is_same_v<TT_easeFunctor<T_parameter>, None<T_parameter>>)
        {
            w.witness(NVP(mEaseFunctor));
        }
    }

private:
    T_parameter mPeriod;
    TT_periodicity<T_parameter>
        mPeriodicBehaviour; // empty class for basic initial cases (Repeat,
                            // PingPong) but leaves room for more potential
                            // other scenarios.
    TT_easeFunctor<T_parameter> mEaseFunctor;

    // Would be better to have the asseration at the beginning, but the static
    // member functions must be defined.
    static_assert(!IsTrivial(),
                  "Please use TrivialParameterAnimation for the trivial case.");
};

/// \brief Partial specialization for the **trivial** case of a full-range
/// non-easing non-periodic parameter animation.
///
/// It amounts to multiplying the accumulated input by the speed.
template <class T_parameter>
class ParameterAnimation<T_parameter, FullRange, None, None>
    : public detail::ParameterAnimation_ResultHelper<T_parameter, FullRange>
{
    using Base_t =
        detail::ParameterAnimation_ResultHelper<T_parameter, FullRange>;
    using Base_t::mSpeed;

public:
    explicit ParameterAnimation(T_parameter aSpeed) : Base_t{std::move(aSpeed)}
    {}

    typename Base_t::Result_type at(T_parameter aInput) const
    {
        return aInput * mSpeed;
    }

    static constexpr bool HasSpeed() { return true; }

    static constexpr bool IsFinite() { return false; }

    constexpr bool isCompleted(T_parameter aInput) const { return false; }

    constexpr T_parameter getOvershoot(T_parameter aInput) const
    {
        return T_parameter{0};
    }
};

/// \brief Helper factory for ParameterAnimation, taking a period.
template <AnimationResult N_resultRange,
          template <class> class TT_periodicity = None,
          template <class> class TT_easeFunctor = None,
          class T_parameter>
ParameterAnimation<T_parameter, N_resultRange, TT_periodicity, TT_easeFunctor>
makeParameterAnimation(T_parameter aSingleParameter)
{
    return ParameterAnimation<T_parameter, N_resultRange, TT_periodicity,
                              TT_easeFunctor>{aSingleParameter};
}

#define PARAMETER_ANIMATION                                                    \
    ParameterAnimation<T_parameter, N_resultRange, TT_periodicity,             \
                       TT_easeFunctor>

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
    return ParameterAnimation<T_parameter, N_resultRange, TT_periodicity,
                              TT_easeFunctor>{aPeriod, aSpeed};
}

#undef PARAMETER_ANIMATION

} // namespace math
} // namespace ad

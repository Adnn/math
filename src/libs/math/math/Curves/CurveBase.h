#pragma once


#include "../Vector.h"

#include <array>


namespace ad {
namespace math {


// Implementer note:
// T_derived is only used to make sure different derived curve types do not have a common type.
// (e.g. preventing cross comparisons)
#define TMP class T_derived, int N_controlPoints, int N_pointDimension, class T_number
#define TMA T_derived, N_controlPoints, N_pointDimension, T_number


/// \brief Base class for curve, providing common members.
template <TMP>
class CurveBase
{
    static_assert(N_controlPoints >= 2, "Curves are supported from degree 1 onward.");

public:
    static constexpr int size_v = N_controlPoints;
    static constexpr int degree_v = N_controlPoints - 1;

    using Position_t = Position<N_pointDimension, T_number>;

    template <class... T_position,
              std::enable_if_t<sizeof...(T_position) == N_controlPoints, int> = 0>
    constexpr CurveBase(T_position... vaControlPoints);

    Position_t & operator[](std::size_t aIndex);
    Position_t operator[](std::size_t aIndex) const;

    Position_t & start()
    { return (*this)[0]; }
    Position_t start() const
    { return (*this)[0]; }
    Position_t & end()
    { return (*this)[N_controlPoints - 1]; }
    Position_t end() const
    { return (*this)[N_controlPoints - 1]; }

    constexpr const Position_t * data() const noexcept;

    constexpr bool operator==(const CurveBase & aRhs) const
    { return mControlPoints == aRhs.mControlPoints; }
    constexpr bool operator!=(const CurveBase & aRhs) const
    { return ! (*this == aRhs); }

private:
    std::array<Position_t, N_controlPoints> mControlPoints;
};


template <class T_curve, class T_parameter>
constexpr typename T_curve::Position_t evaluateBlending(const T_curve & aCurve, T_parameter aParameter);


//
// Implementations
//
template<TMP>
template <class... T_position,
          std::enable_if_t<sizeof...(T_position) == N_controlPoints, int>>
constexpr CurveBase<TMA>::CurveBase(T_position... vaControlPoints) :
        mControlPoints{ {vaControlPoints...} }
{}


template<TMP>
typename CurveBase<TMA>::Position_t & CurveBase<TMA>::operator[](std::size_t aIndex)
{
    return mControlPoints[aIndex];
}


template<TMP>
typename CurveBase<TMA>::Position_t CurveBase<TMA>::operator[](std::size_t aIndex) const
{
    return mControlPoints[aIndex];
}


template<TMP>
constexpr const typename CurveBase<TMA>::Position_t * CurveBase<TMA>::data() const noexcept
{
    return mControlPoints.data();
}


template <TMP>
typename CurveBase<TMA>::Position_t evaluate(CurveBase<TMA> aCurve, T_number aParameter)
{
    // Uses the CurveBase object as storage, hence the copy
    for (std::size_t step = 1; step != N_controlPoints; ++step)
    {
        for (std::size_t pointId = 0; pointId != N_controlPoints - step; ++pointId)
        {
            // This is a simple lerp, not including the header atm.
            aCurve[pointId] =
                aCurve[pointId] + aParameter * (aCurve[pointId + 1] - aCurve[pointId]);
        }
    }
    return aCurve[0];
}


template <TMP>
std::pair<CurveBase<TMA>, CurveBase<TMA>> subdivide(CurveBase<TMA> aCurve, T_number aParameter)
{
    // Would be enough to only copy first element, but we cannot make unitialized CurveBase.
    CurveBase<TMA> left = aCurve;
    for (std::size_t step = 1; step != N_controlPoints; ++step)
    {
        left[step] = left[step - 1] + aParameter * (aCurve[1] - left[step - 1]);
        for (std::size_t pointId = 1; pointId != N_controlPoints - step; ++pointId)
        {
            // This is a simple lerp, not including the header atm.
            aCurve[pointId] =
                aCurve[pointId] + aParameter * (aCurve[pointId + 1] - aCurve[pointId]);
        }
    }
    aCurve[0] = left[N_controlPoints - 1];
    return {left, aCurve};
}


template <class T_curve, class T_parameter>
constexpr typename T_curve::Position_t evaluateBlending(const T_curve & aCurve, T_parameter aParameter)
{
    auto coeffs = aCurve.getBlendingCoefficients(aParameter);
    auto evaluated = T_curve::Position_t::Zero();
    for (int controlId = 0; controlId != T_curve::size_v; ++controlId)
    {
        evaluated += aCurve[controlId].template as<math::Vec>() * coeffs[controlId];
    }
    return evaluated;
}


#undef TMA
#undef TMP


} // namespace math
} // namespace ad
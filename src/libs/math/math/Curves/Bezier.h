#pragma once


#include "CurveBase.h"

#include <utility> // for std::pair


namespace ad {
namespace math {


#define TMP int N_controlPoints, int N_pointDimension, class T_number
#define TMP_D int N_controlPoints, int N_pointDimension, class T_number = double
#define TMA N_controlPoints, N_pointDimension, T_number


/// \brief Model a Bézier curve of arbitrary degree, in a space of arbitrary dimension.
template <TMP_D>
class Bezier : public CurveBase<Bezier<TMA>, TMA>
{
public:
    using CurveBase<Bezier, TMA>::CurveBase;
};


/// \brief Evaluate the curve position at given parameter value.
template <TMP>
typename Bezier<TMA>::Position_t evaluate(Bezier<TMA> aBezier, T_number aParameter);


/// \brief Subdivide the curve in two subcurves, using De Casteljau algorithm.
template <TMP>
std::pair<Bezier<TMA>, Bezier<TMA>> subdivide(Bezier<TMA> aBezier, T_number aParameter);


//
// Implementations
//
template <TMP>
typename Bezier<TMA>::Position_t evaluate(Bezier<TMA> aBezier, T_number aParameter)
{
    // Uses the Bezier object as storage, hence the copy
    for (std::size_t step = 1; step != N_controlPoints; ++step)
    {
        for (std::size_t pointId = 0; pointId != N_controlPoints - step; ++pointId)
        {
            // This is a simple lerp, not including the header atm.
            aBezier[pointId] =
                aBezier[pointId] + aParameter * (aBezier[pointId + 1] - aBezier[pointId]);
        }
    }
    return aBezier[0];
}


template <TMP>
std::pair<Bezier<TMA>, Bezier<TMA>> subdivide(Bezier<TMA> aBezier, T_number aParameter)
{
    // Would be enough to only copy first element, but we cannot make unitialized Bezier.
    Bezier<TMA> left = aBezier;
    for (std::size_t step = 1; step != N_controlPoints; ++step)
    {
        left[step] = left[step - 1] + aParameter * (aBezier[1] - left[step - 1]);
        for (std::size_t pointId = 1; pointId != N_controlPoints - step; ++pointId)
        {
            // This is a simple lerp, not including the header atm.
            aBezier[pointId] =
                aBezier[pointId] + aParameter * (aBezier[pointId + 1] - aBezier[pointId]);
        }
    }
    aBezier[0] = left[N_controlPoints - 1];
    return {left, aBezier};
}


#undef TMA
#undef TMP_D
#undef TMP


} // namespace math
} // namespace ad

#pragma once


#include "../Vector.h"

#include <array>


namespace ad {
namespace math {


#define TMP int N_curveDegree, int N_dimension, class T_number
#define TMP_D int N_curveDegree, int N_dimension, class T_number = double
#define TMA N_curveDegree, N_dimension, T_number


/// \brief Model a Bézier curve of arbitrary degree, in a space of arbitrary dimension.
template <TMP_D>
class Bezier
{
    static_assert(N_curveDegree > 1, "Bezier curves are supported from degree 2 onward.");

public:
    using Position_t = Position<N_dimension, T_number>;

    template <class... T_position,
              std::enable_if_t<sizeof...(T_position) == N_curveDegree, int> = 0>
    constexpr Bezier(T_position... vaControlPoints);

    Position_t & operator[](std::size_t aIndex);
    Position_t operator[](std::size_t aIndex) const;

    Position_t & start()
    { return (*this)[0]; }
    Position_t start() const
    { return (*this)[0]; }
    Position_t & end()
    { return (*this)[N_curveDegree - 1]; }
    Position_t end() const
    { return (*this)[N_curveDegree - 1]; }

    constexpr const Position_t * data() const noexcept;

private:
    std::array<Position_t, N_curveDegree> mControlPoints;
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
template<TMP>
template <class... T_position,
          std::enable_if_t<sizeof...(T_position) == N_curveDegree, int>>
constexpr Bezier<TMA>::Bezier(T_position... vaControlPoints) :
        mControlPoints{ {vaControlPoints...} }
{}


template<TMP>
typename Bezier<TMA>::Position_t & Bezier<TMA>::operator[](std::size_t aIndex)
{
    return mControlPoints[aIndex];
}


template<TMP>
typename Bezier<TMA>::Position_t Bezier<TMA>::operator[](std::size_t aIndex) const
{
    return mControlPoints[aIndex];
}


template<TMP>
constexpr const typename Bezier<TMA>::Position_t * Bezier<TMA>::data() const noexcept
{
    return mControlPoints.data();
}


template <TMP>
typename Bezier<TMA>::Position_t evaluate(Bezier<TMA> aBezier, T_number aParameter)
{
    // Uses the Bezier object as storage, hence the copy
    for (std::size_t step = 1; step != N_curveDegree; ++step)
    {
        for (std::size_t pointId = 0; pointId != N_curveDegree - step; ++pointId)
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
    for (std::size_t step = 1; step != N_curveDegree; ++step)
    {
        left[step] = left[step - 1] + aParameter * (aBezier[1] - left[step - 1]);
        for (std::size_t pointId = 1; pointId != N_curveDegree - step; ++pointId)
        {
            // This is a simple lerp, not including the header atm.
            aBezier[pointId] =
                aBezier[pointId] + aParameter * (aBezier[pointId + 1] - aBezier[pointId]);
        }
    }
    aBezier[0] = left[N_curveDegree - 1];
    return {left, aBezier};
}


#undef TMA
#undef TMP


} // namespace math
} // namespace ad

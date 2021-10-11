#pragma once


#include "Bezier.h"
#include "CurveBase.h"


namespace ad {
namespace math {


#define TMP int N_pointDimension, class T_number
#define TMP_D int N_pointDimension, class T_number = double
#define TMA N_pointDimension, T_number

template <TMP_D>
class CardinalCubic : public CurveBase<CardinalCubic<TMA>, 4, N_pointDimension, T_number>
{
    using Base = CurveBase<CardinalCubic, 4, N_pointDimension, T_number>;
public:
    using Position_t = typename Base::Position_t;

    template <class... T_position>
    constexpr CardinalCubic(T_number aTension, T_position... vaControlPoints) :
        Base{vaControlPoints...},
        mSFactor{computeSFactor(aTension)}
    {}

    constexpr bool operator==(const CardinalCubic & aRhs) const
    { 
        return static_cast<const Base &>(*this) == static_cast<const Base &>(aRhs) 
               && mSFactor == aRhs.mSFactor; 
    }
    constexpr bool operator!=(const CardinalCubic & aRhs) const
    { return ! (*this == aRhs); }


    constexpr math::Vec<4, T_number> getBlendingCoefficients(T_number aParameter) const;

    constexpr Bezier<4, N_pointDimension, T_number> toBezier() const;
    
    constexpr Position_t evaluate(T_number aParameter) const
    { return evaluateBlending(*this, aParameter); }

private:
    static T_number computeSFactor(T_number aTension);

    T_number mSFactor;
};


//
// Implementations
//
template <TMP>
T_number CardinalCubic<TMA>::computeSFactor(T_number aTension)
{
    return (T_number{1} - aTension) / T_number{2};
}


template <TMP>
constexpr math::Vec<4, T_number> CardinalCubic<TMA>::getBlendingCoefficients(T_number aParameter) const
{
    const T_number t = aParameter;
    const T_number s = mSFactor;
    const T_number t2 = std::pow(t, 2);
    const T_number t3 = std::pow(t, 3);

    return {
            -s * t  + (2 * s) * t2      - s * t3,
        1.f         + (s - 3) * t2      + (2 - s) * t3,
            s * t   + (3 - 2 * s) * t2  + (s - 2) * t3,
                    - s * t2            + s * t3
    };
}


template <TMP>
constexpr Bezier<4, N_pointDimension, T_number> CardinalCubic<TMA>::toBezier() const
{
    const T_number s = mSFactor;
    return Bezier<4, N_pointDimension, T_number>{
        Position_t{(*this)[1]},
        Position_t{(*this)[1] + s/3.f * ((*this)[2] - (*this)[0])},
        Position_t{(*this)[2] - s/3.f * ((*this)[3] - (*this)[1])},
        Position_t{(*this)[2]}
    };

}


#undef TMA
#undef TMP_D
#undef TMP


} // namespace math
} // namespace ad

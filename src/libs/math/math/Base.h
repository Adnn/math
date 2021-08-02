#pragma once


#include "Vector.h"


namespace ad {
namespace math {


template <int N_dimension, class T_number = real_number>
class OrthonormalBase;


template <class T_number>
class OrthonormalBase<3, T_number>
{
public:
    static constexpr OrthonormalBase MakeFromSingleVector(Vec<3, T_number> aWDirection);
    static constexpr OrthonormalBase MakeFromTwoVectors(Vec<3, T_number> aWDirection,
                                                         Vec<3, T_number> aUpDirection);

    constexpr UnitVec<3, T_number> u() const noexcept
    { return mU; }
    constexpr UnitVec<3, T_number> v() const noexcept
    { return mV; }
    constexpr UnitVec<3, T_number> w() const noexcept
    { return mW; }

private:
    constexpr OrthonormalBase(UnitVec<3, T_number> aU,
                              UnitVec<3, T_number> aV,
                              UnitVec<3, T_number> aW);

    UnitVec<3, T_number> mU;
    UnitVec<3, T_number> mV;
    UnitVec<3, T_number> mW;
};


template <class T_number>
constexpr OrthonormalBase<3, T_number>::OrthonormalBase(UnitVec<3, T_number> aU,
                                                        UnitVec<3, T_number> aV,
                                                        UnitVec<3, T_number> aW):
    mU{std::move(aU)},
    mV{std::move(aV)},
    mW{std::move(aW)}
{}

template <class T_number>
constexpr OrthonormalBase<3, T_number>
OrthonormalBase<3, T_number>::MakeFromSingleVector(Vec<3, T_number> aWDirection)
{
    using UnitVec = UnitVec<3, T_number>;
    UnitVec w{aWDirection};

    Vec<3, T_number> t = w;
    *t.getMinMagnitudeElement() = static_cast<T_number>(1);
    UnitVec u{t.cross(w)};

    return {u, UnitVec::MakeFromUnitLength(w.cross(u)), w };
}


template <class T_number>
constexpr OrthonormalBase<3, T_number>
OrthonormalBase<3, T_number>::MakeFromTwoVectors(Vec<3, T_number> aWDirection,
                                                  Vec<3, T_number> aUpDirection)
{
    using UnitVec = UnitVec<3, T_number>;
    UnitVec w{aWDirection};

    UnitVec u{aUpDirection.cross(w)};

    return {u, UnitVec::MakeFromUnitLength(w.cross(u)), w };
}


} // namespace math
} // namespace ad

#pragma once


#include "Vector.h"


namespace ad {
namespace math {


template <int N_dimension, class T_number = real_number>
class OrthonormalBase;


/// \brief Helper structure to represent a complete frame of reference
/// (i.e. a coordinate system).
template <int N_dimension, class T_number = real_number>
struct Frame{
    Position<N_dimension, T_number> origin;
    OrthonormalBase<N_dimension, T_number> base;
};


//
// 2D
//

template <class T_number>
class OrthonormalBase<2, T_number>
{
public:
    static constexpr OrthonormalBase MakeFromSingleVector(Vec<2, T_number> aUDirection);

    constexpr UnitVec<2, T_number> u() const noexcept
    { return mU; }
    constexpr UnitVec<2, T_number> v() const noexcept
    { return mV; }

private:
    constexpr OrthonormalBase(UnitVec<2, T_number> aU,
                              UnitVec<2, T_number> aV);

    UnitVec<2, T_number> mU;
    UnitVec<2, T_number> mV;
};


template <class T_number>
constexpr OrthonormalBase<2, T_number>::OrthonormalBase(UnitVec<2, T_number> aU,
                                                        UnitVec<2, T_number> aV) :
        mU{std::move(aU)},
        mV{std::move(aV)}
{}


template <class T_number>
constexpr OrthonormalBase<2, T_number>
OrthonormalBase<2, T_number>::MakeFromSingleVector(Vec<2, T_number> aUDirection)
{
    using UnitVec = UnitVec<2, T_number>;
    UnitVec u{aUDirection};

    return {u, UnitVec::MakeFromUnitLength({-u.y(), u.x()})};
}


//
// 3D
//

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

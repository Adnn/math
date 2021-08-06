#pragma once

#include "Vector.h"

namespace ad {
namespace math {


/// \brief
/// \important The origin is **front** bottom left.
template <class T_number>
struct Box
{
    T_number & x()
    { return mPosition.x(); }
    T_number & y()
    { return mPosition.y(); }
    T_number & z()
    { return mPosition.z(); }

    T_number x() const
    { return mPosition.x(); }
    T_number y() const
    { return mPosition.y(); }
    T_number z() const
    { return mPosition.z(); }

    T_number & width()
    { return mDimension.width(); }
    T_number & height()
    { return mDimension.height(); }
    T_number & depth()
    { return mDimension.depth(); }

    T_number width() const
    { return mDimension.width(); }
    T_number height() const
    { return mDimension.height(); }
    T_number depth() const
    { return mDimension.depth(); }

    T_number xMin() const
    { return x(); }
    T_number xMax() const
    { return x() + width(); }

    T_number yMin() const
    { return y(); }
    T_number yMax() const
    { return y() + height(); }

    T_number zMin() const
    { return z() - depth(); }
    T_number zMax() const
    { return z(); }

    T_number volume() const
    { return mDimension.volume(); }

    Position<3, T_number> origin() const
    { return mPosition; }

    Position<3, T_number> bottomLeftFront() const
    { return origin(); }

    Position<3, T_number> topLeftFront() const
    { return mPosition + Vec<3, T_number>{T_number(0), mDimension.height(), T_number{0}}; }

    Position<3, T_number> bottomRightFront() const
    { return mPosition + Vec<3, T_number>{mDimension.width(), T_number{0}, T_number{0}}; }

    Position<3, T_number> topRightFront() const
    { return mPosition + Vec<3, T_number>{mDimension.width(), mDimension.height(), T_number{0}}; }

    Position<3, T_number> bottomLeftBack() const
    { return mPosition + Vec<3, T_number>{T_number{0}, T_number{0}, - mDimension.depth()}; }

    Position<3, T_number> topLeftBack() const
    { return mPosition + Vec<3, T_number>{T_number{0}, mDimension.height(), - mDimension.depth()}; }

    Position<3, T_number> bottomRightBack() const
    { return mPosition + Vec<3, T_number>{mDimension.width(), T_number{0}, - mDimension.depth()}; }

    Position<3, T_number> topRightBack() const
    { return mPosition + Vec<3, T_number>{mDimension.width(), mDimension.height(), - mDimension.depth()}; }

    bool operator==(const Box &aRhs) const
    { return mPosition == aRhs.mPosition && mDimension == aRhs.mDimension; }
    bool operator!=(const Box &aRhs) const
    { return !(*this == aRhs); }

    Box centered() const;

    template <class T_positionValue>
    bool contains(Position<3, T_positionValue> aPosition) const;

    Position<3, T_number> closestPoint(Position<3, T_number> aPosition) const;

    Position<3, T_number>  mPosition;
    Size<3, T_number> mDimension;
};


template <class T_number>
Box<T_number> Box<T_number>::centered() const
{ 
    auto dimensionDepthNegated{mDimension};
    dimensionDepthNegated.depth() = - dimensionDepthNegated.depth();

    return { {mPosition - static_cast<Vec<3, T_number>>(dimensionDepthNegated/T_number{2})},
            mDimension };
}


template <class T_number>
template <class T_positionValue>
bool Box<T_number>::contains(Position<3, T_positionValue> aPosition) const
{
    return (aPosition.x() >= xMin())
        && (aPosition.y() >= yMin())
        && (aPosition.z() >= zMin())
        && (aPosition.x() <= xMax())
        && (aPosition.y() <= yMax())
        && (aPosition.z() <= zMax())
        ;
}


template <class T_number>
Position<3, T_number> Box<T_number>::closestPoint(Position<3, T_number> aPosition) const
{
    aPosition.x() = std::min(std::max(aPosition.x(), xMin()), xMax());
    aPosition.y() = std::min(std::max(aPosition.y(), yMin()), yMax());
    aPosition.z() = std::min(std::max(aPosition.z(), zMin()), zMax());
    return aPosition;
}


}} // namespace ad::math

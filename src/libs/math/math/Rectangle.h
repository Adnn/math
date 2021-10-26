#pragma once

#include "Vector.h"

namespace ad {
namespace math {


template <class T_number>
struct Rectangle
{
    T_number & x()
    { return mPosition.x(); }
    T_number & y()
    { return mPosition.y(); }

    T_number x() const
    { return mPosition.x(); }
    T_number y() const
    { return mPosition.y(); }

    T_number xMin() const
    { return x(); }
    T_number xMax() const
    { return x() + width(); }

    T_number yMin() const
    { return y(); }
    T_number yMax() const
    { return y() + height(); }

    T_number & width()
    { return mDimension.width(); }
    T_number & height()
    { return mDimension.height(); }

    T_number width() const
    { return mDimension.width(); }
    T_number height() const
    { return mDimension.height(); }

    Size<2, T_number> & dimension()
    { return mDimension; }
    Size<2, T_number> dimension() const
    { return mDimension; }

    T_number area() const
    { return mDimension.area(); }

    Position<2, T_number> & origin()
    { return mPosition; }
    Position<2, T_number> origin() const
    { return mPosition; }

    Position<2, T_number> bottomLeft() const
    { return origin(); }

    Position<2, T_number> bottomRight() const
    { return mPosition + Vec<2, T_number>{mDimension.width(), T_number{0}}; }

    Position<2, T_number> topLeft() const
    { return mPosition + Vec<2, T_number>{T_number{0}, mDimension.height()}; }

    Position<2, T_number> topRight() const
    // TODO: Why as does not work on clang here ?!
    //{ return mPosition + mDimension.as<Vec>(); }
    { return mPosition + static_cast<Vec<2, T_number>>(mDimension); }

    Position<2, T_number> center() const
    { return mPosition + static_cast<Vec<2, T_number>>(mDimension / 2); }

    bool operator==(const Rectangle &aRhs) const
    { return mPosition == aRhs.mPosition && mDimension == aRhs.mDimension; }
    bool operator!=(const Rectangle &aRhs) const
    { return !(*this == aRhs); }

    Rectangle centered() const
    { return { {mPosition - static_cast<Vec<2, T_number>>(mDimension/T_number{2})},
                mDimension }; }

    /// \brief Construct a Rectangle of provided dimension, centered on origin (0, 0).
    static Rectangle CenterOnOrigin(Size<2, T_number> aDimension);

    template <class T_positionValue>
    bool contains(Position<2, T_positionValue> aPosition) const;

    Position<2, T_number> closestPoint(Position<2, T_number> aPosition) const;

    Position<2, T_number>  mPosition;
    Size<2, T_number> mDimension;
};


template <class T_number>
Rectangle<T_number> Rectangle<T_number>::CenterOnOrigin(Size<2, T_number> aDimension)
{
    return Rectangle{ {T_number{0}, T_number{0}}, aDimension }.centered();
}


template <class T_number>
template <class T_positionValue>
bool Rectangle<T_number>::contains(Position<2, T_positionValue> aPosition) const
{
    return (aPosition.x() >= xMin())
        && (aPosition.y() >= yMin())
        && (aPosition.x() <= xMax())
        && (aPosition.y() <= yMax());
}


template <class T_number>
Position<2, T_number> Rectangle<T_number>::closestPoint(Position<2, T_number> aPosition) const
{
    aPosition.x() = std::min(std::max(aPosition.x(), xMin()), xMax());
    aPosition.y() = std::min(std::max(aPosition.y(), yMin()), yMax());
    return aPosition;
}


template <class T_number>
std::ostream & operator<<(std::ostream & os, const Rectangle<T_number> &aRectangle)
{
    return os << "[ {" << aRectangle.origin() << "}, {" << aRectangle.dimension() << "} ]";
}


}} // namespace ad::math

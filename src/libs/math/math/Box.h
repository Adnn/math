#pragma once

#include "Homogeneous.h"
#include "Rectangle.h"
#include "Vector.h"

namespace ad {
namespace math {


// TODO constexpr this class
/// \important The origin is the bottom left **back** in a right handed frame.
template <class T_number>
struct Box
{
    static constexpr std::size_t gCornerCount = 8;
    static constexpr std::size_t gBorderCount = 6;

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

    T_number xMin() const
    { return x(); }
    T_number xMax() const
    { return x() + width(); }

    T_number yMin() const
    { return y(); }
    T_number yMax() const
    { return y() + height(); }

    T_number zMin() const
    { return z(); }
    T_number zMax() const
    { return z() + depth(); }

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

    Size<3, T_number> & dimension()
    { return mDimension; }
    Size<3, T_number> dimension() const
    { return mDimension; }

    T_number volume() const
    { return mDimension.volume(); }

    Position<3, T_number> & origin()
    { return mPosition; }
    Position<3, T_number> origin() const
    { return mPosition; }

    // Note: There is the ambiguity of handedness on the near/far distinction
    // OpenGL clipping space is left-handed, we might adhere to left-handedness (i.e. near = zMin)
    // But we prefer to be explicit and use ZMin / ZMax for this dimension.

    constexpr Position<3, T_number> leftBottomZMin() const
    { return origin(); }

    constexpr Position<3, T_number> rightBottomZMin() const
    { return mPosition + Vec<3, T_number>{mDimension.width(), T_number{0}, T_number{0}}; }

    constexpr Position<3, T_number> leftTopZMin() const
    { return mPosition + Vec<3, T_number>{T_number(0), mDimension.height(), T_number{0}}; }
    
    constexpr Position<3, T_number> rightTopZMin() const
    { return mPosition + Vec<3, T_number>{mDimension.width(), mDimension.height(), T_number{0}}; }

    constexpr Position<3, T_number> leftBottomZMax() const
    { return mPosition + Vec<3, T_number>{T_number{0}, T_number{0}, mDimension.depth()}; }

    constexpr Position<3, T_number> rightBottomZMax() const
    { return mPosition + Vec<3, T_number>{mDimension.width(), T_number{0}, mDimension.depth()}; }

    constexpr Position<3, T_number> leftTopZMax() const
    { return mPosition + Vec<3, T_number>{T_number{0}, mDimension.height(), mDimension.depth()}; }

    constexpr Position<3, T_number> rightTopZMax() const
    { return mPosition + Vec<3, T_number>{mDimension.width(), mDimension.height(), mDimension.depth()}; }

    /// \return Corner position in the same order as the declarations above
    /// i.e. [l, r] x [b, t] x [zMin, zMax]
    constexpr Position<3, T_number> cornerAt(std::size_t aCornerIdx) const;

    /// \return The plane border component along its natural axis, in the order l, r, b, t, n, f.
    ///
    /// Since the Box is implicitly axis aligned, each plane is entirely defined by its position along the axis.
    /// Each plane defines a **border** between the inside and the outside of the Box.
    constexpr T_number borderAt(std::size_t aBorderIdx) const;

    constexpr Position<3, T_number> center() const
    { return mPosition + (mDimension.template as<Vec>() / 2); }

    bool operator==(const Box &aRhs) const
    { return mPosition == aRhs.mPosition && mDimension == aRhs.mDimension; }
    bool operator!=(const Box &aRhs) const
    { return !(*this == aRhs); }

    static Box Zero();

    Box centered() const;

    /// \brief Construct a Box of provided dimension at the origin, i.e. mPosition == (0, 0, 0).
    static Box AtOrigin(Size<3, T_number> aDimension);

    /// \brief Construct a Box of provided dimension, centered on origin (0, 0, 0).
    static Box CenterOnOrigin(Size<3, T_number> aDimension);

    Box & operator*=(T_number aScalarFactor);

    Box & operator*=(const LinearMatrix<3, 3, T_number> & aTransform);
    Box & operator*=(const AffineMatrix<4, T_number> & aTransform);

    template <class T_positionValue>
    bool contains(Position<3, T_positionValue> aPosition) const;

    /// \brief If the box does not include `aPosition`, grow it just enough so it does.
    void extendTo(Position<3, T_number> aPosition);

    // Note: union is a reserved keywoard
    Box unite(Box aOther) const;
    Box & uniteAssign(const Box & aOther);

    Position<3, T_number> closestPoint(Position<3, T_number> aPosition) const;

    constexpr Rectangle<T_number> frontRectangle() const;

    Position<3, T_number>  mPosition;
    Size<3, T_number> mDimension;

    template<class T_witness>
    void describeTo(T_witness && w)
    {
        w.witness(std::make_pair("mPosition", &mPosition));
        w.witness(std::make_pair("mDimension", &mDimension));
    }
};


template <class T_number>
Box<T_number> operator*(Box<T_number> aBox, T_number aScalarFactor);

template <class T_number>
Box<T_number> operator*(T_number aScalarFactor, Box<T_number> aBox);

template <class T_number>
Box<T_number> operator*(Box<T_number> aBox, const LinearMatrix<3, 3, T_number> & aTransform);

template <class T_number>
Box<T_number> operator*(Box<T_number> aBox, const AffineMatrix<4, T_number> & aTransform);


//
// Implementations
//
template <class T_number>
Box<T_number> Box<T_number>::Zero()
{
    T_number z{0};
    return {{z, z, z}, {z, z, z}};
}


template <class T_number>
Box<T_number> Box<T_number>::centered() const
{ 
    return { {mPosition - static_cast<Vec<3, T_number>>(mDimension/T_number{2})},
            mDimension };
}


template <class T_number>
Box<T_number> Box<T_number>::AtOrigin(Size<3, T_number> aDimension)
{
    return Box{ {T_number{0}, T_number{0}, T_number{0}}, aDimension };
}


template <class T_number>
Box<T_number> Box<T_number>::CenterOnOrigin(Size<3, T_number> aDimension)
{
    return AtOrigin(aDimension).centered();
}


template <class T_number>
Box<T_number> & Box<T_number>::operator*=(T_number aScalarFactor)
{
    mPosition *= aScalarFactor;
    mDimension *= aScalarFactor;
    return *this;
}


template <class T_number>
Box<T_number> & Box<T_number>::operator*=(const LinearMatrix<3, 3, T_number> & aTransform)
{
    // see: https://dev.theomader.com/transform-bounding-boxes/
    // see: http://www.realtimerendering.com/resources/GraphicsGems/gems/TransBox.c
    Position<3, T_number> right{
        aTransform[0][0],
        aTransform[0][1],
        aTransform[0][2],
    };
    auto xa = xMin() * right;
    auto xb = xMax() * right;

    Position<3, T_number> up{
        aTransform[1][0],
        aTransform[1][1],
        aTransform[1][2],
    };
    auto ya = yMin() * up;
    auto yb = yMax() * up;

    Position<3, T_number> back{
        aTransform[2][0],
        aTransform[2][1],
        aTransform[2][2],
    };
    auto za = zMin() * back;
    auto zb = zMax() * back;

    Position<3, T_number> origin = min(xa, xb) + min(ya, yb).template as<Vec>() + min(za, zb).template as<Vec>();
    Position<3, T_number> end =  max(xa, xb) + max(ya, yb).template as<Vec>() + max(za, zb).template as<Vec>();

    mPosition = origin;
    mDimension = (end-origin).template as<Size>();

    return *this;
}


template <class T_number>
Box<T_number> & Box<T_number>::operator*=(const AffineMatrix<4, T_number> & aTransform)
{
    (*this) *= LinearMatrix<3, 3, T_number>{
        aTransform[0][0], aTransform[0][1], aTransform[0][2],
        aTransform[1][0], aTransform[1][1], aTransform[1][2],
        aTransform[2][0], aTransform[2][1], aTransform[2][2],
    };

    mPosition += aTransform.getAffine();

    return *this;
}


template <class T_number>
Box<T_number> operator*(Box<T_number> aBox, T_number aScalarFactor)
{
    return aBox *= aScalarFactor;
}


template <class T_number>
Box<T_number> operator*(T_number aScalarFactor, Box<T_number> aBox)
{
    return aBox * aScalarFactor;
}


template <class T_number>
Box<T_number> operator*(Box<T_number> aBox, const LinearMatrix<3, 3, T_number> & aTransform)
{
    return aBox *= aTransform;
}


template <class T_number>
Box<T_number> operator*(Box<T_number> aBox, const AffineMatrix<4, T_number> & aTransform)
{
    return aBox *= aTransform;
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


// TODO Ad 2022/03/27: I suspect there is room for optimization
// in the extension and boolean operations implementations.
template <class T_number>
void Box<T_number>::extendTo(Position<3, T_number> aPosition)
{
    if (aPosition.x() < xMin()) 
    {
        mDimension.width() = xMax() - aPosition.x();
        mPosition.x() = aPosition.x();
    }
    else if (aPosition.x() > xMax())
    {
        mDimension.width() = aPosition.x() - xMin();
    }

    if (aPosition.y() < yMin()) 
    {
        mDimension.height() = yMax() - aPosition.y();
        mPosition.y() = aPosition.y();
    }
    else if (aPosition.y() > yMax())
    {
        mDimension.height() = aPosition.y() - yMin();
    }

    if (aPosition.z() < zMin())
    {
        mDimension.depth() = zMax() - aPosition.z();
        mPosition.z() = aPosition.z();
    }
    else if (aPosition.z() > zMax()) 
    {
        mDimension.depth() = aPosition.z() - zMin();
    }
}


template <class T_number>
Box<T_number> Box<T_number>::unite(Box aOther) const
{
   return aOther.uniteAssign(*this);
}


template <class T_number>
Box<T_number> & Box<T_number>::uniteAssign(const Box & aOther)
{
    extendTo(aOther.leftBottomZMax());
    extendTo(aOther.rightTopZMin());
    return *this;
}


template <class T_number>
Position<3, T_number> Box<T_number>::closestPoint(Position<3, T_number> aPosition) const
{
    aPosition.x() = std::min(std::max(aPosition.x(), xMin()), xMax());
    aPosition.y() = std::min(std::max(aPosition.y(), yMin()), yMax());
    aPosition.z() = std::min(std::max(aPosition.z(), zMin()), zMax());
    return aPosition;
}


template <class T_number>
constexpr Rectangle<T_number> Box<T_number>::frontRectangle() const
{
    return {
        {mPosition.x(), mPosition.y()},
        {mDimension.width(), mDimension.height()}
    };
}


template <class T_number>
constexpr Position<3, T_number> Box<T_number>::cornerAt(std::size_t aCornerIdx) const
{
    switch(aCornerIdx)
    {
        case 0:
            return leftBottomZMin();
        case 1:
            return rightBottomZMin();
        case 2:
            return leftTopZMin();
        case 3:
            return rightTopZMin();
        case 4:
            return leftBottomZMax();
        case 5:
            return rightBottomZMax();
        case 6:
            return leftTopZMax();
        case 7:
            return rightTopZMax();
        default:
            throw std::domain_error{__func__ + std::string{": box corners are indexed (0, 7)."}};
    }
}


template <class T_number>
constexpr T_number Box<T_number>::borderAt(std::size_t aBorderIdx) const
{
    switch(aBorderIdx)
    {
        case 0:
            return xMin();
        case 1:
            return xMax();
        case 2:
            return yMin();
        case 3:
            return yMax();
        case 4:
            return zMin();
        case 5:
            return zMax();
        default:
            throw std::domain_error{__func__ + std::string{": box corners are indexed (0, 7)."}};
    }
}

template <class T_number>
std::ostream & operator<<(std::ostream & os, const Box<T_number> &aBox)
{
    return os << "[ {" << aBox.origin() << "}, {" << aBox.dimension() << "} ]";
}


}} // namespace ad::math

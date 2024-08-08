#pragma once

namespace ad {
namespace math {


//
// Member functions
//
template <class T_number>
constexpr Quaternion<T_number>::Quaternion(T_number aX, T_number aY, T_number aZ, T_number aW)
noexcept(should_noexcept) :
    mVector{aX, aY, aZ},
    mW{aW}
{}


template <class T_number>
constexpr Quaternion<T_number>::Quaternion(Vec<3, T_number> aVector, T_number aW)
noexcept(should_noexcept) :
    mVector{aVector},
    mW{aW}
{}


template <class T_number>
template <class T_unitTag>
constexpr Quaternion<T_number>::Quaternion(UnitVec<3, T_number> aRotationAxis,
                                           Angle<T_number, T_unitTag> aRotationAngle)
noexcept(should_noexcept) :
    mVector{aRotationAxis * sin(aRotationAngle / 2)},
    mW{cos(aRotationAngle / 2)}
{}


template <class T_number>
T_number Quaternion<T_number>::getNormSquared() const
{
    return asVec().getNormSquared();
}


template <class T_number>
constexpr Quaternion<T_number> Quaternion<T_number>::Identity() noexcept(should_noexcept)
{
    return {T_number{0}, T_number{0}, T_number{0}, T_number{1}};
}


template <class T_number>
constexpr bool Quaternion<T_number>::operator==(const Quaternion &aRhs) const noexcept(should_noexcept)
{
    return mVector == aRhs.mVector && mW == aRhs.mW;
}


template <class T_number>
constexpr bool Quaternion<T_number>::operator!=(const Quaternion &aRhs) const noexcept(should_noexcept)
{
    return !(*this == aRhs);
}


template <class T_number>
constexpr T_number & Quaternion<T_number>::x() noexcept(should_noexcept)
{ return mVector.x(); }


template <class T_number>
constexpr T_number & Quaternion<T_number>::y() noexcept(should_noexcept)
{ return mVector.y(); }


template <class T_number>
constexpr T_number & Quaternion<T_number>::z() noexcept(should_noexcept)
{ return mVector.z(); }


template <class T_number>
constexpr T_number & Quaternion<T_number>::w() noexcept(should_noexcept)
{ return mW; }


template <class T_number>
constexpr T_number Quaternion<T_number>::x() const noexcept(should_noexcept)
{ return mVector.x(); }


template <class T_number>
constexpr T_number Quaternion<T_number>::y() const noexcept(should_noexcept)
{ return mVector.y(); }


template <class T_number>
constexpr T_number Quaternion<T_number>::z() const noexcept(should_noexcept)
{ return mVector.z(); }


template <class T_number>
constexpr T_number Quaternion<T_number>::w() const noexcept(should_noexcept)
{ return mW; }



template <class T_number>
constexpr Quaternion<T_number> Quaternion<T_number>::operator-() 
const noexcept(should_noexcept)
{
    return {-mVector, -mW};
}


template <class T_number>
constexpr Quaternion<T_number> & Quaternion<T_number>::operator*=(const Quaternion & aRhs) 
noexcept(should_noexcept)
{
    T_number w = mW * aRhs.mW - mVector.dot(aRhs.mVector);
    mVector = mW * aRhs.mVector + aRhs.mW * mVector + mVector.cross(aRhs.mVector);
    mW = w;
    return *this;
}


template <class T_number>
constexpr Quaternion<T_number> Quaternion<T_number>::conjugate() const noexcept(should_noexcept)
{
    return {-mVector, mW};
}


template <class T_number>
constexpr Quaternion<T_number> Quaternion<T_number>::inverse() const noexcept(should_noexcept)
{
    // IMPORTANT This is true only because we expect Quaternion to be a unit quaternion.
    return conjugate(); 
}


template <class T_number>
template <class T_derived> 
//requires (is_position_v<T_derived> || is_vec_v<T_derived>)
T_derived Quaternion<T_number>::rotate(const Vector<T_derived, 3, T_number> & aVector) const
noexcept(should_noexcept)
{
    return static_cast<T_derived>(
        (*this * Quaternion{aVector.template as<Vec>(), T_number{0}} * this->conjugate()).mVector);
}


template <class T_number>
constexpr LinearMatrix<3, 3, T_number> Quaternion<T_number>::toRotationMatrix() const 
noexcept(should_noexcept)
{
    return {
        //1                                 //2                                 //3
        1 - 2 * y() * y() - 2 * z() * z(),  2 * x() * y() + 2 * z() * w(),      2 * x() * z() - 2 * y() * w(),
        2 * x() * y() - 2 * z() * w(),      1 - 2 * x() * x() - 2 * z() * z(),  2 * y() * z() + 2 * x() * w(),
        2 * x() * z() + 2 * y() * w(),      2 * y() * z() - 2 * x() * w(),      1 - 2 * x() * x() - 2 * y() * y(),
    };
}


template <class T_number>
constexpr bool 
Quaternion<T_number>::equalsWithinTolerance(const Quaternion & aRhs, T_number aEpsilon) const
noexcept(should_noexcept)
{
    return mVector.equalsWithinTolerance(aRhs.mVector, aEpsilon) 
        && std::abs(mW - aRhs.mW) <= aEpsilon;
}


template <class T_number>
constexpr Vec<4, T_number> Quaternion<T_number>::asVec() const 
noexcept(should_noexcept)
{
    return {mVector, mW};
}


//
// Free functions
//
template <class T_number>
constexpr Quaternion<T_number>
operator*(Quaternion<T_number> aLhs, const Quaternion<T_number> & aRhs) 
noexcept(decltype(aLhs)::should_noexcept)
{
    return aLhs *= aRhs;
}


template <class T_number>
constexpr Quaternion<T_number>
difference(Quaternion<T_number> aLhs, const Quaternion<T_number> & aRhs) 
noexcept(decltype(aLhs)::should_noexcept)
{
    return aRhs * aLhs.inverse();
}


template <class T_number>
T_number getCosineHalfAngle(Quaternion<T_number> aLhs, const Quaternion<T_number> & aRhs)
noexcept(decltype(aLhs)::should_noexcept)
{
    return aLhs.asVec().dot(aRhs.asVec());
}


template <class T_number>
std::ostream & operator<<(std::ostream & aOut, const Quaternion<T_number> & aQuaternion)
{
    return aOut << "<q>{" 
        << aQuaternion.x() << ", "
        << aQuaternion.y() << ", "
        << aQuaternion.z() << ", " 
        << aQuaternion.w()
        << "}"
        ;
}


template <class T_number>
Quaternion<T_number> toQuaternion(const LinearMatrix<3, 3, T_number> & m)
{
    // See: 3dmpfgagd 2nd p286 listing 8.5
    std::array<T_number, 4> fourTimesSquaredMinusOne = {{
        m.at(0, 0) + m.at(1, 1) + m.at(2, 2),
        m.at(0, 0) - m.at(1, 1) - m.at(2, 2),
        m.at(1, 1) - m.at(0, 0) - m.at(2, 2),
        m.at(2, 2) - m.at(0, 0) - m.at(1, 1),
    }};

    auto maxElementIt = 
        std::max_element(std::begin(fourTimesSquaredMinusOne),
                         std::end(fourTimesSquaredMinusOne));

    std::size_t maxIdx = maxElementIt - std::begin(fourTimesSquaredMinusOne);
    T_number maxVal = std::sqrt(*maxElementIt + 1) / T_number(2);
    T_number d = 4 * maxVal;

    T_number x, y, z, w;
    switch(maxIdx)
    {
        case 0:
            w = maxVal;
            x = (m.at(1, 2) - m.at(2, 1)) / d;
            y = (m.at(2, 0) - m.at(0, 2)) / d;
            z = (m.at(0, 1) - m.at(1, 0)) / d;
            break;
        case 1:
            x = maxVal;
            w = (m.at(1, 2) - m.at(2, 1)) / d;
            y = (m.at(0, 1) + m.at(1, 0)) / d;
            z = (m.at(2, 0) + m.at(0, 2)) / d;
            break;
        case 2:
            y = maxVal;
            w = (m.at(2, 0) - m.at(0, 2)) / d;
            x = (m.at(0, 1) + m.at(1, 0)) / d;
            z = (m.at(1, 2) + m.at(2, 1)) / d;
            break;
        case 3:
            z = maxVal;
            w = (m.at(0, 1) - m.at(1, 0)) / d;
            x = (m.at(2, 0) + m.at(0, 2)) / d;
            y = (m.at(1, 2) + m.at(2, 1)) / d;
            break;
    }

    Quaternion<T_number> result{x, y, z, w};
    // Ensure the resulting quaternion does represent a rotation, with a tolerance.
    assert(absoluteTolerance(result.getNormSquared(),
                             T_number{1},
                             std::numeric_limits<T_number>::epsilon() * T_number{1E1}));
    return result;
}


} // namespace math
} // namespace ad

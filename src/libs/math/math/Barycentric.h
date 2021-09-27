#pragma once


#include "Vector.h"


namespace ad {
namespace math {


/// \brief Barycentric coordinate system for 2D triangles
template <class T_number>
class Barycentric
{
    struct Factors
    {
        T_number x;
        T_number y;
        T_number constant;
    };

public:
    /// \brief Represent the barycentric coordinates of a 2D point
    struct Coordinates
    {
        T_number alpha;
        T_number beta;
        T_number gamma;

        // Implementation note: has to be a member function instead of a free function:
        // On a free function, T_number could not be deduced from a type nested in a 
        // dependent template class
        bool operator==(const Coordinates &aRhs) const;
    };

    /// \attention Undefined behaviour if the three points form a degenerate triangle.
    Barycentric(Position<2, T_number> aPointA, Position<2, T_number> aPointB, Position<2, T_number> aPointC);

    /// \brief Compute the barycentric coordinates of aPoint in the basis formed by this Barycentric
    Coordinates getCoordinates(Position<2, T_number> aPoint);
    
private:
    static T_number signedDistance(const Factors &aFactors, Position<2, T_number> aPoint);


private:
    Factors mBetaFactors;
    Factors mGammaFactors;

    T_number mValueB;
    T_number mValueC;
};


template <class T_number>
Barycentric<T_number>::Barycentric(Position<2, T_number> aPointA, Position<2, T_number> aPointB, Position<2, T_number> aPointC) :
        mBetaFactors { 
            aPointA.y()-aPointC.y(),
            aPointC.x()-aPointA.x(),
            aPointA.x()*aPointC.y() - aPointC.x()*aPointA.y()
        },
        mGammaFactors {
            aPointA.y()-aPointB.y(),
            aPointB.x()-aPointA.x(),
            aPointA.x()*aPointB.y() - aPointB.x()*aPointA.y()
        },
        mValueB{ signedDistance(mBetaFactors, aPointB) },
        mValueC{ signedDistance(mGammaFactors, aPointC) }
{}


template <class T_number>
Barycentric<T_number> makeBarycentric(Position<2, T_number> aPointA, Position<2, T_number> aPointB, Position<2, T_number> aPointC)
{
    return Barycentric<T_number>(aPointA, aPointB, aPointC);
}


template <class T_number>
typename Barycentric<T_number>::Coordinates Barycentric<T_number>::getCoordinates(Position<2, T_number> aPoint)
{
    static_assert(std::is_floating_point<T_number>::value, "Not yet implement for non-floating point coordinates");

    T_number beta = signedDistance(mBetaFactors, aPoint) / mValueB;
    T_number gamma = signedDistance(mGammaFactors, aPoint) / mValueC;
    return { (1-beta-gamma), beta, gamma };
}


template <class T_number>
T_number Barycentric<T_number>::signedDistance(const Factors &aFactors, Position<2, T_number> aPoint)
{
    return aPoint.x() * aFactors.x + aPoint.y() * aFactors.y + aFactors.constant;
}


/***
 * Coordinates
 ***/

template <class T_number>
bool Barycentric<T_number>::Coordinates::operator==(const Barycentric::Coordinates & aRhs) const
{
    return (alpha == aRhs.alpha) && (beta == aRhs.beta) && (gamma == aRhs.gamma);
}


}} // namespace ad::math

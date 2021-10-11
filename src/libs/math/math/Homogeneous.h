#pragma once


#include "commons.h"
#include "LinearMatrix.h"
#include "Matrix.h"
#include "Vector.h"

#include <utility>


namespace ad {
namespace math {


#define TMP int N_dimension, class T_number
#define TMP_D int N_dimension, class T_number = real_number
#define TMA N_dimension, T_number


// Implementer note:
// Ideally, the `AffineMatrix` would not store the final [0..0 1] elements.
// Yet that implies taking special care for all operations that need to access it (e.g. isDiagonal(), etc.),
// and also if client were to directly access it / try to provide a contiguous sequence with all elements (e.g. OpenGL buffer).
// For simplicity of first implementation, store all the values explicitly.

/// \brief A square matrix of dimension N_dimension x N_dimension, that can represent linear and affine transformations
/// of homogeneous coordinates.
///
/// \important Its projective elements are fixed as [0..0 1], it cannot be used to implement perspective projection.
///
/// \warning It is undefined behaviour to modify the projective elements of an AffineMatrix.
template <TMP_D>
class AffineMatrix : public Matrix<N_dimension, N_dimension, T_number>
{
    static_assert(N_dimension > 1, "AffineMatrix dimension must be >= 2.");

    using base_type = Matrix<N_dimension, N_dimension, T_number>;
    using base_type::should_noexcept;

    // Important:
    // The constructor from base_type are not inherited, to avoid the constructor
    // accepting an explicit list of elements.

public:
    /*implicit*/ constexpr AffineMatrix(const LinearMatrix<N_dimension-1, N_dimension-1, T_number> & aLinear,
                                        const Vec<N_dimension-1, T_number> & aAffine =
                                            Vec<N_dimension-1, T_number>::Zero())
        noexcept(should_noexcept);

    // Implementer note:
    // Cannot find a way to implement that, because the [0..0 1] values should be inserted in the middle
    // of the parameter pack expansion.
    //template <class... VT_elements,
    //          class /*enabler*/ = std::enable_if_t<sizeof...(VT_elements) == N_dimension * (N_dimension - 1)>>
    //constexpr AffineMatrix(VT_elements && ... aElements) noexcept(should_noexcept);

    // Note: It would probably be better to have the explicit element constructor instead.
    explicit constexpr AffineMatrix(const Matrix<N_dimension, N_dimension-1, T_number> & aElements)
        noexcept(should_noexcept);

    explicit constexpr AffineMatrix(typename base_type::UninitializedTag) noexcept(should_noexcept);

    //
    // Disable the modifying operations incompatible with AffineMatrix specialized case
    //
    base_type & operator+=(const Matrix<N_dimension, N_dimension, T_number> & aRhs) = delete;
    base_type & operator-=(const Matrix<N_dimension, N_dimension, T_number> & aRhs) = delete;
    base_type & operator*=(const Matrix<N_dimension, N_dimension, T_number> & aRhs) = delete;
    base_type & operator/=(const Matrix<N_dimension, N_dimension, T_number> & aRhs) = delete;
    base_type & setZero() = delete;
    static base_type Zero() = delete;


    //
    // Factories
    //
    static constexpr AffineMatrix Identity() noexcept(should_noexcept);

    //
    // Arithmetic operations
    //
    constexpr AffineMatrix & operator*=(const AffineMatrix & aRhs) noexcept(should_noexcept);
    // Must be a defined inside the class to allow implicit conversions on both operands.
    // see: https://stackoverflow.com/a/9789036/1027706
    friend constexpr AffineMatrix<TMA> operator*(const AffineMatrix &aLhs, const AffineMatrix &aRhs)
    { return aLhs.multiply_impl(aRhs); }

private:
    constexpr AffineMatrix<TMA> multiply_impl(const AffineMatrix<TMA> &aRhs) const;
};

#undef TMP_D


// Implementer note:
// I wonder if the homogeneous vector types should be separate types
// (whether derived, or independently implemented).
// This could offer some more special case optimization safely verified by the type system.
// Yet it would complicate the class hierarchy even more.
//
// For the moment just use the normal vector types, and provide helper
// factories to put the final 0 or 1 accordingly.
//
// WARNING: without dedicated type handling, currently the Vector::as<> conversion
// produces wrong results when used to convert homogeneous Position<->Vec
// (the final 0/1 is not switched).


namespace homogeneous
{

/// \brief Factory for homogeneous positions.
template<int N_dimension, class T_number = real_number, class... VT_elements>
Position<N_dimension, T_number> makePosition(VT_elements && ... aElements)
{
    return Position<N_dimension, T_number>{std::forward<VT_elements>(aElements)..., T_number{1}};
}

template<int N_lowerDimension, class T_number = real_number>
Position<N_lowerDimension+1, T_number> makePosition(const Position<N_lowerDimension, T_number> & aPosition)
{
    return Position<N_lowerDimension+1, T_number>{aPosition, T_number{1}};
}

/// \brief Factory for homogeneous vectors (in the sense of "displacement").
template<int N_dimension, class T_number = real_number, class... VT_elements>
Vec<N_dimension, T_number> makeVec(VT_elements && ... aElements)
{
    return Vec<N_dimension, T_number>{std::forward<VT_elements>(aElements)..., T_number{0}};
}

template<int N_lowerDimension, class T_number = real_number>
Vec<N_lowerDimension+1, T_number> makeVec(const Vec<N_lowerDimension, T_number> & aPosition)
{
    return Vec<N_lowerDimension+1, T_number>{aPosition, T_number{1}};
}


//template<int N_dimension, class T_number = real_number, template <N_dimension, T_number> TT_derivedVector>
template<class T_derived, int N_dimension, class T_number = real_number>
T_derived normalize(const Vector<T_derived, N_dimension, T_number> & aHomogeneous)
{
    T_derived result{typename T_derived::UninitializedTag{}};
    for (std::size_t i = 0; i != (N_dimension - 1); ++i)
    {
        result[i] = aHomogeneous[i]/aHomogeneous[N_dimension - 1];
    }
    result[N_dimension - 1] = T_number{1};
    return result;
}




} // namespace homogeneous


} // namespace math
} // namespace ad


#include "Homogeneous-impl.h"


#undef TMA
#undef TMP

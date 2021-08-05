#pragma once


#include "commons.h"
#include "Matrix.h"
#include "Vector.h"

#include <utility>


namespace ad {
namespace math {


#define TMA int N_dimension, class T_number
#define TMA_D int N_dimension, class T_number = real_number
#define TMP N_dimension, T_number


// Implementer note:
// Ideally, the `AffineMatrix` would not store the final [0..0 1] elements.
// Yet that implies taking special care for all operations that need to access it (e.g. isDiagonal(), etc.),
// and also if client were to directly access it / try to provide a contiguous sequence with all elements (e.g. OpenGL buffer).
// For simplicity of first implementation, store all the values explicitly.

/// \brief A square matrix of dimension N_dimension x N_dimension, that can represent linear and affine transformations
/// of homogeneous coordinates.
///
/// \important Its projective elements are fixed as [0..0 1], it cannot be used to implement perspective projection.
template <TMA_D>
class AffineMatrix : public Matrix<N_dimension, N_dimension, T_number>
{
    using base_type = Matrix<N_dimension, N_dimension, T_number>;
    using base_type::should_noexcept;

    // Important:
    // The constructor from base_type are not inherited, to avoid the constructor
    // accepting an explicit list of elements.

public:
    explicit constexpr AffineMatrix(const Matrix<N_dimension-1, N_dimension-1, T_number> & aLinear,
                                    const Vec<N_dimension-1, T_number> & aAffine =
                                        Vec<N_dimension-1, T_number>::Zero())
        noexcept(should_noexcept);

    explicit constexpr AffineMatrix(typename base_type::UninitializedTag) noexcept(should_noexcept);

    base_type & operator+=(const Matrix<N_dimension, N_dimension, T_number> & aRhs) = delete;
    base_type & operator-=(const Matrix<N_dimension, N_dimension, T_number> & aRhs) = delete;
    base_type & operator*=(const Matrix<N_dimension, N_dimension, T_number> & aRhs) = delete;
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
};

#undef TMA_D


/// \brief Affine matrices multiplication.
template <TMA>
constexpr AffineMatrix<TMP>
operator*(const AffineMatrix<TMP> &aLhs, const AffineMatrix<TMP> &aRhs);


// Implementer note:
// I wonder if the homogeneous vector types should be separate types
// (whether derived, or independently implemented).
// This could offer some more special case optimization safely verified by the type system.
// Yet it would complicate the class hierarchy even more.
//
// For the moment just use the normal vector types, and provide helper
// factories to put the final 0 or 1 accordingly.


namespace homogeneous
{

/// \brief Factory for homogeneous positions.
template<int N_dimension, class T_number = real_number, class... VT_elements>
Position<N_dimension, T_number> makePosition(VT_elements && ... aElements)
{
    return Position<N_dimension, T_number>{std::forward<VT_elements>(aElements)..., T_number{1}};
}

/// \brief Factory for homogeneous vectors (in the sense of "displacement").
template<int N_dimension, class T_number = real_number, class... VT_elements>
Vec<N_dimension, T_number> makeVec(VT_elements && ... aElements)
{
    return Vec<N_dimension, T_number>{std::forward<VT_elements>(aElements)..., T_number{0}};
}

} // namespace homogeneous


} // namespace math
} // namespace ad


#include "Homogeneous-impl.h"


#undef TMP
#undef TMA

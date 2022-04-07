#include "Utilities.h"

namespace ad {
namespace math {


    //base_type{
    //    aLinear[0][0], aLinear[0][1], aLinear[0][2], T_number{0},
    //    aLinear[1][0], aLinear[1][1], aLinear[1][2], T_number{0},
    //    aLinear[2][0], aLinear[2][1], aLinear[2][2], T_number{0},
    //    aAffine[0],    aAffine[1],    aAffine[3],    T_number{1},
    //}


#define FILL_LAST_COLUMN(matrix) \
    for (std::size_t row = 0; row != N_dimension-1; ++row)  \
    {                                                       \
        (matrix)[row][N_dimension-1] = T_number{0};         \
    }                                                       \
    (matrix)[N_dimension-1][N_dimension-1] = T_number{1};


#define FILL(linear, affine) \
    for (std::size_t row = 0; row != N_dimension-1; ++row)      \
    {                                                           \
        for (std::size_t col = 0; col != N_dimension-1; ++col)  \
        {                                                       \
            (*this)[row][col] = (linear)[row][col];             \
        }                                                       \
        (*this)[row][N_dimension-1] = 0;                        \
    }                                                           \
    for (std::size_t col = 0; col != N_dimension-1; ++col)      \
    {                                                           \
        (*this)[N_dimension-1][col] = (affine)[col];            \
    }                                                           \
    (*this)[N_dimension-1][N_dimension-1] = 1;                  


template <TMP>
constexpr AffineMatrix<TMA>::AffineMatrix(const LinearMatrix<N_dimension-1, N_dimension-1, T_number> & aLinear,
                                          const Vec<N_dimension-1, T_number> & aAffine) noexcept(should_noexcept) :
        base_type{typename base_type::UninitializedTag{}}
{
    FILL(aLinear, aAffine)
}


template <TMP>
constexpr AffineMatrix<TMA>::AffineMatrix(const Matrix<N_dimension, N_dimension-1, T_number> & aElements)
    noexcept(should_noexcept) :
        base_type{typename base_type::UninitializedTag{}}
{
    FILL(aElements, aElements[N_dimension-1])
}


template <TMP>
constexpr AffineMatrix<TMA>::AffineMatrix(typename base_type::UninitializedTag) noexcept(should_noexcept) :
    base_type{typename base_type::UninitializedTag{}}
{}


template <TMP>
constexpr Vec<N_dimension - 1, T_number> AffineMatrix<TMA>::getAffine() const noexcept(should_noexcept)
{
    return this->template extract<Vec<N_dimension - 1, T_number>>(
        makeIndexSequence<N_dimension - 1, 
                          N_dimension * (N_dimension - 1)>()
    );
}


template <TMP>
constexpr AffineMatrix<TMA> AffineMatrix<TMA>::Identity() noexcept(should_noexcept)
{
    return AffineMatrix{LinearMatrix<N_dimension-1, N_dimension-1, T_number>::Identity()};
}


template <TMP>
constexpr AffineMatrix<TMA>
AffineMatrix<TMA>::multiply_impl(const AffineMatrix<TMA> &aRhs) const
{
    AffineMatrix<TMA> result = multiplyBaseSubrange<AffineMatrix<TMA>, N_dimension, N_dimension-1>(*this, aRhs);
    FILL_LAST_COLUMN(result);
    return result;
}


template <TMP>
constexpr AffineMatrix<TMA> & AffineMatrix<TMA>::operator*=(const AffineMatrix & aRhs) noexcept(should_noexcept)
{
    *this = *this * aRhs;
    return *this;
}


#undef FILL_LAST_COLUMN


} // namespace math
} // namespace ad

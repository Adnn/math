#pragma once


#include "commons.h"
#include "Matrix.h"


namespace ad {
namespace math {


#define TMP int N_rows, int N_cols, class T_number
#define TMP_D int N_rows, int N_cols, class T_number = real_number
#define TMA N_rows, N_cols, T_number


/// \brief This class should behave exactly like a Matrix of matching dimension
/// but can be to explicitly provide more properties statically.
///
/// For e.g. this is usefull when it comes to multiplication with affine matrices,
/// which can safely be done via implicit conversion.
template <TMP_D>
class LinearMatrix : public ::ad::math::Matrix<TMA>
{
    using base_type = ::ad::math::Matrix<TMA>;
    // Ctor inheritance
    using base_type::base_type;

public:
    template<class T>
    using derived_type = LinearMatrix<N_rows, N_cols, T>;
};


template <TMP>
constexpr LinearMatrix<TMA>
operator*(const LinearMatrix<TMA> &aLhs, const LinearMatrix<TMA> &aRhs)
{
    return multiplyBase<LinearMatrix<TMA>>(aLhs, aRhs);
}


#undef TMA
#undef TMP_D
#undef TMP


} // namespace math
} // namespace ad

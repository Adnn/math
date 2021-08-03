#pragma once


#include "commons.h"
#include "MatrixBase.h"


namespace ad {
namespace math {


#define TMA int N_rows, int N_cols, class T_number
#define TMP N_rows, N_cols, T_number


template <int N_rows, int N_cols, class T_number=real_number>
class Matrix : public MatrixBase<Matrix<TMP>, N_rows, N_cols, T_number>
{
    typedef MatrixBase<Matrix<TMP>, N_rows, N_cols, T_number> base_type;
    using base_type::base_type;
    using base_type::should_noexcept;

public:

    template<class T>
    using derived_type = Matrix<TMP>;

    static constexpr bool is_square_value{N_rows == N_cols};

    static constexpr Matrix Identity() noexcept(should_noexcept);

    /// \brief Return another matrix instance, which is the transpose of this matrix.
    constexpr Matrix<N_cols, N_rows, T_number> transpose() const noexcept(should_noexcept);

    using base_type::operator*=;
    constexpr Matrix & operator*=(const Matrix & aRhs) noexcept(should_noexcept);
};







#include "Matrix-impl.h"


#undef TMP
#undef TMA


}} // namespace ad::math

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

    // Implementer note:
    // T_dummy template argument is required so the `enable_if` conditions depends on a deduced template argument.
    /// \brief Test if the matrix is diagonal (i.e. only diagonal elements might not be null).
    template <class T_dummy = int>
    constexpr std::enable_if_t<sizeof(T_dummy) && is_square_value, bool> isDiagonal() const noexcept(should_noexcept)
    { return isDiagonal_impl(); }

    /// \brief Test if the matrix is symmetric (the matrix is equal to its transpose, by definition).
    template <class T_dummy = int>
    constexpr std::enable_if_t<sizeof(T_dummy) && is_square_value, bool> isSymmetric() const noexcept(should_noexcept)
    { return isSymmetric_impl(); }

    /// \brief Return another matrix instance, which is the transpose of this matrix.
    constexpr Matrix<N_cols, N_rows, T_number> transpose() const noexcept(should_noexcept);

    using base_type::operator*=;
    constexpr Matrix & operator*=(const Matrix & aRhs) noexcept(should_noexcept);

private:
    constexpr bool isDiagonal_impl() const noexcept(should_noexcept);
    constexpr bool isSymmetric_impl() const noexcept(should_noexcept);
};







#include "Matrix-impl.h"


#undef TMP
#undef TMA


}} // namespace ad::math

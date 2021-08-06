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
protected:
    using base_type::should_noexcept;

public:

    template<class T>
    using derived_type = Matrix<N_rows, N_cols, T>;

    static constexpr bool is_square_value{N_rows == N_cols};

    static constexpr Matrix Identity() noexcept(should_noexcept);

    /// \brief Test if the matrix is diagonal (i.e. only diagonal elements might not be null).
    constexpr bool isDiagonal() const noexcept(should_noexcept);

    /// \brief Test if the matrix is symmetric (the matrix is equal to its transpose, by definition).
    constexpr bool isSymmetric() const noexcept(should_noexcept);

    /// \brief Compute the trace of this matrix (i.e. the sum of its diagonal elements).
    /// \note This matrix must be square.
    constexpr T_number trace() const noexcept(should_noexcept);

    /// \brief Return another matrix instance, which is the transpose of this matrix.
    constexpr Matrix<N_cols, N_rows, T_number> transpose() const noexcept(should_noexcept);

    /// \brief Return another matrix, which is the inverse of this matrix.
    /// \attention  The current implementation is very inefficient for large matrices
    constexpr Matrix inverse() const noexcept(should_noexcept);

    /// \brief Compute the determinant of this matrix.
    constexpr T_number determinant() const noexcept(should_noexcept);

    /// \brief Return the submatrix obtained by removing `aRemovedRow`th row and `aRemovedColumn`th column from this matrix.
    constexpr Matrix<N_rows-1, N_cols-1, T_number>
    getSubmatrix(std::size_t aRemovedRow, std::size_t aRemovedColumn) const noexcept(should_noexcept);

    /// \brief Compute the cofactor of the matrix element `at(aRow, aColumn)`.
    constexpr T_number cofactor(std::size_t aRow, std::size_t aColumn) const noexcept(should_noexcept);

    /// \brief Return another matrix, which has the corresponding cofactor in the place of each element of this matrix.
    constexpr Matrix computeCofactorMatrix() const noexcept(should_noexcept);

    /// \brief Return another matrix to transform surface's normals, if this matrix transformed the surface itself.
    /// \attention This matrix will only give the correct direction, it will arbitrarily change the length of the normal.
    constexpr Matrix toNormalTansformScaled() const noexcept(should_noexcept)
    { return computeCofactorMatrix(); }

    /// \brief Return another matrix, which is the adjoint matrix (i.e. the transpose of the cofactor matrix).
    constexpr Matrix computeAdjointMatrix() const noexcept(should_noexcept);

    //
    // Arithmetic operations
    //
    using base_type::operator*=; // For scalar compound multiplication

    constexpr Matrix & operator*=(const Matrix<N_cols, N_cols, T_number> & aRhs) noexcept(should_noexcept);
};


/// \brief Matrix multiplication
template <int N_lRows, int N_matching, int N_rCols, class T_number>
constexpr Matrix<N_lRows, N_rCols, T_number>
operator*(const Matrix<N_lRows, N_matching, T_number> &aLhs,
          const Matrix<N_matching, N_rCols, T_number> &aRhs);


namespace detail
{

    template <class T_number>
    constexpr T_number computeDeterminant_impl(const Matrix<1, 1, T_number> & aMatrix);

    template <class T_number>
    constexpr T_number computeDeterminant_impl(const Matrix<2, 2, T_number> & aMatrix);

    template <class T_number, int N_dimension>
    constexpr T_number computeDeterminant_impl(const Matrix<N_dimension, N_dimension, T_number> & aMatrix);

} // namespace detail


}} // namespace ad::math


#include "Matrix-impl.h"


#undef TMP
#undef TMA

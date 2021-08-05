namespace ad {
namespace math {


template <TMA>
constexpr Matrix<TMP>
Matrix<TMP>::Identity() noexcept(should_noexcept)
{
    static_assert(is_square_value, "Only square matrices can be identity.");

    Matrix result = Matrix::Zero();
    for(std::size_t index = 0; index != N_rows; ++index)
    {
        result[index][index] = 1;
    }
    return result;
}


template <TMA>
constexpr bool
Matrix<TMP>::isDiagonal() const noexcept(should_noexcept)
{
    static_assert(is_square_value, "Only square matrices could be diagonal.");

    for(std::size_t row = 0; row != N_rows; ++row)
    {
        for(std::size_t col = 0; col != N_cols; ++col)
        {
            if ((row != col) && (this->at(row, col) != 0))
            {
                return false;
            }
        }
    }
    return true;
}


template <TMA>
constexpr bool
Matrix<TMP>::isSymmetric() const noexcept(should_noexcept)
{
    static_assert(is_square_value, "Only square matrices could be symmetric.");

    for(std::size_t row = 0; row != N_rows; ++row)
    {
        for(std::size_t col = 1 + row; col != N_cols; ++col)
        {
            if (this->at(row, col) != this->at(col, row))
            {
                return false;
            }
        }
    }
    return true;
}


template <TMA>
constexpr T_number Matrix<TMP>::trace() const noexcept(should_noexcept)
{
    static_assert(is_square_value, "Trace can only be computed for square matrices.");

    T_number result = 0;
    for (std::size_t index = 0; index != N_cols; ++index)
    {
        result += (*this)[index][index];
    }
    return result;
}


template <TMA>
constexpr Matrix<N_cols, N_rows, T_number>
Matrix<TMP>::transpose() const noexcept(should_noexcept)
{
    using result_type = Matrix<N_cols, N_rows, T_number>;
    result_type result{typename result_type::UninitializedTag{}};
    for(std::size_t sourceRow = 0; sourceRow != N_rows; ++sourceRow)
    {
        for(std::size_t sourceCol = 0; sourceCol != N_cols; ++sourceCol)
        {
            result[sourceCol][sourceRow] = (*this)[sourceRow][sourceCol];
        }
    }
    return result;
}


template <TMA>
constexpr Matrix<TMP> Matrix<TMP>::inverse() const noexcept(should_noexcept)
{
    static_assert(is_square_value, "Only square matrices are invertible.");
    // See FoCG 3rd p101.
    return computeAdjointMatrix() / determinant();
}


template <TMA>
constexpr T_number Matrix<TMP>::determinant() const noexcept(should_noexcept)
{
    static_assert(is_square_value, "Determinant is only available on square matrices.");
    return detail::computeDeterminant_impl(*this);
}


template <TMA>
constexpr Matrix<N_rows-1, N_cols-1, T_number>
Matrix<TMP>::getSubmatrix(std::size_t aRemovedRow, std::size_t aRemovedColumn) const noexcept(should_noexcept)
{
    static_assert(N_rows > 1 && N_cols > 1, "Cannot get submatrix when starting from a dimension < 2x2.");

    using MatrixResult = Matrix<N_rows-1, N_cols-1, T_number>;
    MatrixResult result{typename MatrixResult::UninitializedTag{}};

    for(std::size_t row = 0; row != N_rows-1; ++row)
    {
        for(std::size_t col = 0; col != N_cols-1; ++col)
        {
            result[row][col] = (*this)[row < aRemovedRow ? row : row+1][col < aRemovedColumn ? col : col+1];
        }
    }

    return result;
}


template <TMA>
constexpr T_number
Matrix<TMP>::cofactor(std::size_t aRow, std::size_t aColumn) const noexcept(should_noexcept)
{
    static_assert(is_square_value, "Cofactors can only be computed on square matrices.");
    // See FoCG 3rd p100.
    return static_cast<int>(std::pow(-1, aRow+aColumn)) * getSubmatrix(aRow, aColumn).determinant();
}


template <TMA>
constexpr Matrix<TMP> Matrix<TMP>::computeCofactorMatrix() const noexcept(should_noexcept)
{
    Matrix result{typename Matrix::UninitializedTag{}};
    for(std::size_t row = 0; row != N_rows; ++row)
    {
        for(std::size_t col = 0; col != N_cols; ++col)
        {
            result[row][col] = cofactor(row, col);
        }
    }
    return result;
}


template <TMA>
constexpr Matrix<TMP> Matrix<TMP>::computeAdjointMatrix() const noexcept(should_noexcept)
{
    return computeCofactorMatrix().transpose();
}


/// The explicit N_rowsMultiplied and N_colsMultiplied template arguments allow to implement subset multiplications
/// (useful for optimizing Affined matrices with known [0..0 1] elements).
/// For complete multiplication, use multiplyBase() directly.
template <class T_result, int N_rowsMultiplied, int N_colsMultiplied, int N_lRows, int N_matching, int N_rCols, class T_lDerived, class T_number>
constexpr T_result multiplyBaseSubrange(const MatrixBase<T_lDerived, N_lRows, N_matching, T_number> &aLhs,
                                     const Matrix<N_matching, N_rCols, T_number> &aRhs)
{
    T_result result{typename T_result::UninitializedTag{}};
    for(std::size_t row = 0; row != N_rowsMultiplied; ++row)
    {
        for(std::size_t col = 0; col != N_colsMultiplied; ++col)
        {
            // Inner multiplication
            for(std::size_t index = 0; index != N_matching; ++index)
            {
                // Uses at() on result instead of double subscript, because T_result may be Vector type.
                result.at(row, col) += aLhs[row][index] * aRhs[index][col];
            }

        }
    }
    return result;
}


template <class T_result, int N_lRows, int N_matching, int N_rCols, class T_lDerived, class T_number>
constexpr T_result multiplyBase(const MatrixBase<T_lDerived, N_lRows, N_matching, T_number> &aLhs,
                                const Matrix<N_matching, N_rCols, T_number> &aRhs)
{
    // Here, subrange is chosen to be the totality.
    return multiplyBaseSubrange<T_result, N_lRows, N_rCols>(aLhs, aRhs);
}


template <int N_lRows, int N_matching, int N_rCols, class T_number>
constexpr Matrix<N_lRows, N_rCols, T_number>
operator*(const Matrix<N_lRows, N_matching, T_number> &aLhs,
          const Matrix<N_matching, N_rCols, T_number> &aRhs)
{
    return multiplyBase<Matrix<N_lRows, N_rCols, T_number>>(aLhs, aRhs);
}


template <TMA>
constexpr Matrix<TMP> & Matrix<TMP>::operator*=(const Matrix<N_cols, N_cols, T_number> & aRhs) noexcept(should_noexcept)
{
    *this = *this * aRhs;
    return *this;
}


namespace detail
{
    template <class T_number>
    constexpr T_number computeDeterminant_impl(const Matrix<1, 1, T_number> & aMatrix)
    {
        return aMatrix[0][0];
    }

    template <class T_number>
    constexpr T_number computeDeterminant_impl(const Matrix<2, 2, T_number> & aMatrix)
    {
        return aMatrix[0][0] * aMatrix[1][1] - aMatrix[0][1] * aMatrix[1][0];
    }

    template <class T_number, int N_dimension>
    constexpr T_number computeDeterminant_impl(const Matrix<N_dimension, N_dimension, T_number> & aMatrix)
    {
        T_number accumulator{0};
        for(std::size_t col = 0; col != N_dimension; ++col)
        {
            accumulator += aMatrix.at(0, col) * aMatrix.cofactor(0, col);
        }
        return accumulator;
    }

} // namespace detail


}} // namespace ad::math

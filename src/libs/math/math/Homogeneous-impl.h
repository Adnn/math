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
        (matrix)[row][N_dimension-1] = 0;                   \
    }                                                       \
    (matrix)[N_dimension-1][N_dimension-1] = 1;


template <TMA>
constexpr AffineMatrix<TMP>::AffineMatrix(const Matrix<N_dimension-1, N_dimension-1, T_number> & aLinear,
                                          const Vec<N_dimension-1, T_number> & aAffine) noexcept(should_noexcept) :
    base_type{typename base_type::UninitializedTag{}}
{
    for (std::size_t row = 0; row != N_dimension-1; ++row)
    {
        for (std::size_t col = 0; col != N_dimension-1; ++col)
        {
            (*this)[row][col] = aLinear[row][col];
        }
        (*this)[row][N_dimension-1] = 0;
    }
    for (std::size_t col = 0; col != N_dimension-1; ++col)
    {
        (*this)[N_dimension-1][col] = aAffine[col];
    }
    (*this)[N_dimension-1][N_dimension-1] = 1;
}


template <TMA>
constexpr AffineMatrix<TMP>::AffineMatrix(typename base_type::UninitializedTag) noexcept(should_noexcept) :
    base_type{typename base_type::UninitializedTag{}}
{}

template <TMA>
constexpr AffineMatrix<TMP> AffineMatrix<TMP>::Identity() noexcept(should_noexcept)
{
    return AffineMatrix{Matrix<N_dimension-1, N_dimension-1, T_number>::Identity()};
}


template <TMA>
constexpr AffineMatrix<TMP>
operator*(const AffineMatrix<TMP> &aLhs, const AffineMatrix<TMP> &aRhs)
{
    AffineMatrix<TMP> result = multiplyBaseSubrange<AffineMatrix<TMP>, N_dimension, N_dimension-1>(aLhs, aRhs);
    FILL_LAST_COLUMN(result);
    return result;
}


template <TMA>
constexpr AffineMatrix<TMP> & AffineMatrix<TMP>::operator*=(const AffineMatrix & aRhs) noexcept(should_noexcept)
{
    *this = *this * aRhs;
    return *this;
}


#undef FILL_LAST_COLUMN


} // namespace math
} // namespace ad

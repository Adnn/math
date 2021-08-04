    //base_type{
    //    aLinear[0][0], aLinear[0][1], aLinear[0][2], T_number{0},
    //    aLinear[1][0], aLinear[1][1], aLinear[1][2], T_number{0},
    //    aLinear[2][0], aLinear[2][1], aLinear[2][2], T_number{0},
    //    aAffine[0],    aAffine[1],    aAffine[3],    T_number{1},
    //}

template <TMA>
constexpr AffineMatrix<TMP>::AffineMatrix(const Matrix<N_dimension-1, N_dimension-1, T_number> & aLinear,
                                          const Vec<N_dimension-1, T_number> & aAffine) noexcept(should_noexcept) :
    base_type{base_type::UninitializedTag{}}
{
    for (std::size_t row = 0; row != N_dimension-1; ++row)
    {
        for (std::size_t col = 0; col != N_dimension-1; ++col)
        {
            (*this)[row][col] = aLinear[row][col];
            std::cout << row << " " << col << ": " << aLinear[row][col];
        }
        (*this)[row][N_dimension-1] = 0;
    }
    for (std::size_t col = 0; col != N_dimension-1; ++col)
    {
        (*this)[N_dimension-1][col] = aAffine[col];
    }
    (*this)[N_dimension-1][N_dimension-1] = 1;
}

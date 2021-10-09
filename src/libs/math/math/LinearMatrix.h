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

    static constexpr LinearMatrix Identity() noexcept(should_noexcept);


private:
    // Even though the conversion ctor is explicit and private
    // it is detected via is_staticcastable_t. Adding a dummy tag fixes that.
    struct Dummy{};
    explicit LinearMatrix(base_type aOther, Dummy) : base_type{std::move(aOther)}
    {}
};


template <TMP>
constexpr LinearMatrix<TMA>
operator*(const LinearMatrix<TMA> &aLhs, const LinearMatrix<TMA> &aRhs)
{
    return multiplyBase<LinearMatrix<TMA>>(aLhs, aRhs);
}


template <TMP>
constexpr LinearMatrix<TMA> LinearMatrix<TMA>::Identity() noexcept(should_noexcept)
{
    return LinearMatrix<TMA>{Matrix<TMA>::Identity(), Dummy{}};
}

 
#undef TMA
#undef TMP_D
#undef TMP


} // namespace math
} // namespace ad

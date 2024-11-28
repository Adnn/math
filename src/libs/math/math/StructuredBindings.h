#pragma once

#include "MatrixBase.h"

#include <utility>
#include <type_traits>

#define TMP class T_derived, int N_rows, int N_cols, class T_number
#define TMA T_derived, N_rows, N_cols, T_number

namespace ad {
namespace math {
namespace detail {

// Create a tuple type of N element of type T_number
// from a number N and a type T_number
// From
// https://stackoverflow.com/questions/33511753/how-can-i-generate-a-tuple-of-n-type-ts
template <class T_Matrix>
class MatrixTuple
{
    template <class = std::make_index_sequence<std::decay_t<T_Matrix>::size_value>>
    struct impl;

    template <size_t... Is>
    struct impl<std::index_sequence<Is...>>
    {
        template <size_t>
        using wrap = typename std::decay_t<T_Matrix>::value_type;

        using type = std::tuple<wrap<Is>...>;
    };

  public:
    using type = typename impl<>::type;
};

template <class T_matrix>
using MatrixTuple_t = typename MatrixTuple<T_matrix>::type;

} // namespace detail
} // namespace math
} // namespace ad


namespace std {

template <class T_matrix> requires ::ad::math::from_matrix_v<T_matrix>
class tuple_size<T_matrix> : public integral_constant<size_t, T_matrix::size_value>
{
};

template <size_t I, class T_matrix> requires ::ad::math::from_matrix_v<T_matrix>
class tuple_element<I, T_matrix>
    : public tuple_element<I, ::ad::math::detail::MatrixTuple_t<T_matrix>>
{
};

} // namespace std

namespace ad {
namespace math {

template <std::size_t Index, typename T_matrix>
typename T_matrix::value_type &
get(T_matrix & aMatrix)
{
    static_assert(Index < std::decay_t<T_matrix>::size_value);
    return aMatrix.at(Index);
}

template <std::size_t Index, typename T_matrix>
const typename T_matrix::value_type
get(const T_matrix & aMatrix)
{
    static_assert(Index < std::decay_t<T_matrix>::size_value);
    return aMatrix.at(Index);
}

} // namespace math
} // namespace ad

#undef TMP
#undef TMA

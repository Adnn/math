#pragma once


#include <cmath>


namespace ad {
namespace math {


namespace detail {


template <std::size_t N_offset, std::size_t... VN_indices>
std::index_sequence<(N_offset + VN_indices)...> addSequenceOffset(std::index_sequence<VN_indices...>)
{
    return {};
}


} // namespace detail


template <class T_number>
bool sameSign(T_number aLeft, T_number aRight)
{
    // see: https://stackoverflow.com/a/3038621/1027706
    return std::signbit(aLeft) == std::signbit(aRight);
}


// see: https://stackoverflow.com/a/35625414/1027706
template <std::size_t N_length, std::size_t N_offset = 0>
auto makeIndexSequence()
{
    return detail::addSequenceOffset<N_offset>(std::make_index_sequence<N_length>());
}


}} // namespace ad::math

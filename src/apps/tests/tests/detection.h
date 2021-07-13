/*
 * Implementation from an experimental detection toolkit
 * see: https://en.cppreference.com/w/cpp/experimental/is_detected
 */

#pragma once

template <class...>
using void_t = void;

struct nonesuch
{
    ~nonesuch() = delete;
    nonesuch(nonesuch const&) = delete;
    void operator=(nonesuch const&) = delete;
};

namespace ad {

namespace detail {

template <class Default, class AlwaysVoid,
          template<class...> class Op, class... Args>
struct detector
{
  using value_t = std::false_type;
  using type = Default;
};

template <class Default,
          template<class...> class Op, class... Args>
struct detector<Default, void_t<Op<Args...>>, Op, Args...>
{
  using value_t = std::true_type;
  using type = Op<Args...>;
};

} // namespace detail

template <template<class...> class Op, class... Args>
using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

template <template<class...> class Op, class... Args>
constexpr bool is_detected_v = is_detected<Op, Args...>::value;

} // namespace ad

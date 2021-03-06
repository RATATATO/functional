/*!
  \file compose.h
  \brief Function definitions for function composition.
 */
#ifndef FUNCTIONAL_COMPSE_H_
#define FUNCTIONAL_COMPSE_H_

#include <functional>
#include <type_traits>

namespace functional {

template <typename T>
constexpr std::function<T(T)> id()
  { return [](T t) -> T { return std::forward<T>(t); }; }

} // namespace functional

template <
  typename F, typename G, typename T = typename F::argument_type,
  typename R = typename std::result_of<G(typename F::result_type)>::type
> constexpr std::function<R(T)> operator<(G g, F f)
  { return [f, g](T t) -> R { return g(f(std::forward<T>(t))); }; }

template <
  typename F, typename G, typename T = typename F::argument_type,
  typename R = typename std::result_of<G(typename F::result_type)>::type
> constexpr std::function<R(T)> operator>(F f, G g)
  { return [f, g](T t) -> R { return g(f(std::forward<T>(t))); }; }

#endif


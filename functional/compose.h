/*!
  \file compose.h
  \brief 合成関数を返す演算子operator<と、恒等射を返す関数idの定義。
 */
#ifndef FUNCTIONAL_COMPSE_H_
#define FUNCTIONAL_COMPSE_H_

#include <functional>

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

#endif


/*!
  \file state.h
  \brief The definition and its controls for a state monad.
 */
#ifndef FUNCTIONAL_STATE_H_
#define FUNCTIONAL_STATE_H_

#include <functional>
#include <type_traits>
#include <utility>

namespace functional {

template <typename S, typename T>
using State = std::function<std::pair<T, S>(S)>;

template <typename T, typename S, typename F>
constexpr functional::State<S, T> make_state(F f)
  { return functional::State<S, T>(f); }

template <typename T, typename S, typename F>
constexpr typename std::result_of<F(T)>::type bind(
  functional::State<S, T> state, F f
) {
  return typename std::result_of<F(T)>::type(
    [state, f](S s) {
      auto new_state = state(std::move(s));
      return f(std::move(new_state.first))(std::move(new_state.second));
    }
  );
}

template <typename T, typename S, typename F, typename... Args>
constexpr typename std::result_of<F(T, Args...)>::type bind(
  functional::State<S, T> state, F f, Args&&... args
) {
  return typename std::result_of<F(T, Args...)>::type(
    [&, state, f](S s) {
      auto g = std::bind(f, std::placeholders::_1, std::forward<Args>(args)...);
      auto new_state = state(std::move(s));
      return g(std::move(new_state.first))(std::move(new_state.second));
    }
  );
}

template <typename T, typename S, typename F>
constexpr auto bind_void(functional::State<S, T> state, F f) -> decltype(f()) {
  return decltype(f())(
    [state, f](S s) {
      auto new_state = state(std::move(s));
      return f()(std::move(new_state.second));
    }
  );
}

template <typename T, typename S, typename F, typename... Args>
constexpr auto bind_void(
  functional::State<S, T> state, F f, Args&&... args
) -> decltype(f(args...)) {
  return decltype(f(args...))(
    [&, state, f](S s) {
      auto g = std::bind(f, std::forward<Args>(args)...);
      auto new_state = state(std::move(s));
      return g()(std::move(new_state.second));
    }
  );
}

template <typename T, typename S, typename F>
constexpr functional::State<S, T> fmap(functional::State<S, T> state, F f) {
  return functional::make_state<T, S>(
    [state, f](S s) {
      auto new_state = state(std::move(s));
      return std::make_pair(std::move(f(new_state.first)), new_state.second);
    }
  );
}

} // namespace functional 

#endif


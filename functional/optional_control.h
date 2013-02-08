/*!
  \file optional_control.h
  \brief Functions for boost::optional.
 */
#ifndef FUNCTIONAL_OPTION_CONTROL_H_
#define FUNCTIONAL_OPTION_CONTROL_H_

#include <functional>
#include <boost/optional.hpp>

namespace functional {

template <typename T>
inline bool hasSome(const boost::optional<T>& o) noexcept
  { return o; }

template <typename T, typename... Args>
inline bool hasSome(const boost::optional<T>& o, const Args&... args) noexcept
  { return o && functional::hasSome(args...); }

template <typename T>
inline bool isNone(const boost::optional<T>& o) noexcept
  { return !o; }

template <typename Generator, typename... Arg>
auto inject_optional(
  Generator g, Arg... args
) noexcept -> decltype(
  boost::make_optional(std::move(g(std::forward<Arg>(args)...)))
) {
  try { return boost::make_optional(std::move(g(std::forward<Arg>(args)...))); }
  catch(...) { return boost::none; }
}

template <typename T, typename F>
auto fmap(boost::optional<T>& o, F f) noexcept -> decltype(
  functional::inject_optional(f, std::move(o.get()))
) {
  try {
    if(o) { return functional::inject_optional(f, o.get()); }
    else { return boost::none; }
  } catch(...) { return boost::none; }
}

template <typename T, typename F>
auto fmap(const boost::optional<T>& o, F f) noexcept -> decltype(
  functional::inject_optional(f, std::move(o.get()))
) {
  try {
    if(o) { return functional::inject_optional(f, o.get()); }
    else { return boost::none; }
  } catch(...) { return boost::none; }
}

template <typename T, typename F>
auto fmap(boost::optional<T>&& o, F f) noexcept -> decltype(
  functional::inject_optional(f, std::move(o.get()))
) {
  try {
    if(o) { return functional::inject_optional(f, std::move(o.get())); }
    else { return boost::none; }
  } catch(...) { return boost::none; }
}

template <typename T, typename F>
auto bind(boost::optional<T>& o, F f) noexcept -> decltype(f(o.get())) {
  try { if(o) { return f(o.get()); } else { return boost::none; } }
  catch(...) { return boost::none; }
}

template <typename T, typename F>
auto bind(const boost::optional<T>& o, F f) noexcept -> decltype(f(o.get())) {
  try { if(o) { return f(o.get()); } else { return boost::none; } }
  catch(...) { return boost::none; }
}

template <typename T, typename F>
auto bind(boost::optional<T>&& o, F f) noexcept -> decltype(
  f(std::move(o.get()))
) {
  try { if(o) { return f(std::move(o.get())); } else { return boost::none; } }
  catch(...) { return boost::none; }
}

} // namespace functional

#endif


//! \file
//! \brief SFINAE utilities

#pragma once

#include <type_traits>

#include <upd/type.hpp>

#include "def.hpp"

#define LDP_MAKE_DETECTOR(NAME, TEMPLATE_PARAMETERS, REQUIREMENTS)                                                     \
  template <TEMPLATE_PARAMETERS, REQUIREMENTS> constexpr bool NAME(int) { return true; }                               \
  template <TEMPLATE_PARAMETERS> constexpr bool NAME(...) { return false; }

namespace ldp {
namespace sfinae {

LDP_MAKE_DETECTOR(has_signature_impl, PACK(typename F, typename R, typename... Args),
                  PACK(typename = typename std::enable_if<
                           std::is_convertible<decltype(std::declval<F>()(std::declval<Args>()...)), R>::value>::type))

//! \brief Indicates whether the instances of the provided types to be invocable like functions of the given signature
template <typename, typename> struct has_signature : std::false_type {};
template <typename F, typename R, typename... Args>
struct has_signature<F, R(Args...)> : std::integral_constant<bool, has_signature_impl<F, R, Args...>(0)> {};

//! \brief Require the provided expression to be true
template <bool Expression, typename U = int> using require = typename std::enable_if<Expression, U>::type;

//! \brief Require the given type to be well formed
template <typename T, typename U = int>
using require_t = typename std::remove_reference<decltype(std::declval<T>(), std::declval<U>())>::type;

//! \brief Require the instances of the given type to be invocable on no parameters and return a byte
template <typename F, typename U = int> using require_input_ftor = require<has_signature<F, upd::byte_t()>::value, U>;

//! \brief Require the instances of the given type to be invocable on a byte
template <typename F, typename U = int>
using require_output_ftor = require<has_signature<F, void(upd::byte_t)>::value, U>;

//! \brief Require the given type to be an iterator type to a byte sequence
template <typename T, typename U = int> using require_is_iterator = require_t<typename T::iterator_category, U>;

} // namespace sfinae
} // namespace ldp

#include "undef.hpp" // IWYU pragma: keep

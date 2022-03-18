//! \file
//! \brief Arbitrary function storage and restoration

#pragma once

#include <upd/type.hpp>

#include "../packet.hpp"

namespace ldp {
namespace detail {

//! \brief Base class used to introduce an input functor of any type through virtual dispatch
struct abstract_input_functor_base {
  virtual upd::byte_t operator()() = 0;
};

//! \brief Input functor binder which provides a
template <typename F> struct abstract_input_functor : abstract_input_functor_base {
  F &ftor;

  explicit abstract_input_functor(F &ftor) : ftor{ftor} {}

  upd::byte_t operator()() final { return ftor(); };
};

//! \brief Arbitrary function type
//! \details When forming a pointer to this type, the result may not necessarly be of the same type as 'void *' on Von
//! Neumann architectures
using any_function_t = void();

//! \brief Restore the original type of a callback and call it on the arguments got from the provided input functor
template <typename F, typename Tk>
inline error restore_and_call(any_function_t *callback_ptr, abstract_input_functor_base &&input_ftor) {
  auto &callback = *reinterpret_cast<F *>(callback_ptr);
  auto maybe = Tk{} << input_ftor;
  maybe.map(callback);
  return maybe ? error::OK : maybe.error();
}

//! \brief Common type of 'restore_and_call' template instances
using restorer_t = decltype(restore_and_call<void, void>);

} // namespace detail
} // namespace ldp

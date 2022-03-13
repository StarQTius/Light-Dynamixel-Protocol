//! \file
//! \brief Sequence detection in JIT flow

#pragma once

#include <cstddef>
#include <upd/type.hpp>

#include "detail/packet.hpp"

namespace ldp {
inline namespace v2 {
namespace detail {

//! \brief Counts up the elements of a pattern in 'detail::header' and indicated when this pattern has been detected
template <std::size_t N> struct sentry_impl {
  std::size_t count;

  //! \brief Initialize the internal counter to zero
  sentry_impl() : count{0} {}

  //! \brief Count up the elements of the pattern and indicates if the pattern has been found
  //! \param byte Byte from an input sequence
  //! \return whether the last 'N' bytes this function was called on constitute the pattern (in order)
  bool operator()(upd::byte_t byte) {
    count = header[count] == byte ? count + 1 : (byte == 0xff && count < 3 ? count : 0);
    if (count == N) {
      count = 0;
      return true;
    } else {
      return false;
    }
  }
};

} // namespace detail

//! \brief Detects a full header in a JIT byte stream
//! \details
//!   This is typically used to know whether a header has been received
using sentry = detail::sentry_impl<sizeof detail::header>;

//! \brief Detects a header minus the last byte in a JIT byte stream
//! \details
//!   This is typically used to know whether a stuffing byte must be added or removed
using stuffing_sentry = detail::sentry_impl<sizeof detail::header - 1>;

} // namespace v2
} // namespace ldp

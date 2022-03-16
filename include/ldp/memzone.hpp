//! \file
//! \brief Device memory mapping

#pragma once

#include <cstdint>

namespace ldp {
inline namespace v2 {

//! \brief Type of device memory addresses
using address_t = std::uint16_t;

//! \brief Labels a memory zone of a device
//! \tparam Address Start of the memory zone
//! \tparam T Type associated with the length of the memory zone (ie. the length of the memory zone is 'sizeof (T)')
template <address_t Address, typename T> struct memzone {
  //! \brief Value of the 'Address' template parameter
  constexpr static auto address = Address;

  //! \brief Alias of the 'T' template parameter
  using type = T;
};

} // namespace v2
} // namespace ldp

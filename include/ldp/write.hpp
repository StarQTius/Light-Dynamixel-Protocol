//! \file
//! \brief Write instruction utilities

#pragma once

#include <upd/format.hpp>

#include "memzone.hpp"
#include "packet.hpp"
#include "request.hpp"

namespace ldp {
inline namespace v2 {

//! \brief Request class for a write instruction
//! \tparam Signed_Mode Signed integer representation of the packets involved in the request
//! \tparam T Type of the value to be written
template <upd::signed_mode Signed_Mode, typename T>
using write_t = request<Signed_Mode, ticket<Signed_Mode, packet_id>, address_t, T>;

//! \brief Prepare the content of a write instruction packet
//! \tparam Signed_Mode Signed integer representation of the packet
//! \tparam Address Start of the memory zone to write on
//! \tparam T Type associated with the memory zone
//! \param id Identifier of the target device
//! \param value Value to write
//! \return A request object that holds the necessary data for a write instruction
template <upd::signed_mode Signed_Mode, address_t Address, typename T, typename U>
write_t<Signed_Mode, T> write(upd::signed_mode_h<Signed_Mode>, packet_id id, memzone<Address, T>, const U &value) {
  return write_t<Signed_Mode, T>{id, instruction::WRITE, Address, static_cast<const T &>(value)};
}

//! \copybrief write
//! \tparam Address Start of the memory zone to write on
//! \tparam T Type associated with the memory zone
//! \param id Identifier of the target device
//! \param value Value to write
//! \return A request object that holds the necessary data for a write instruction
template <address_t Address, typename T, typename U>
write_t<upd::signed_mode::TWO_COMPLEMENT, T> write(packet_id id, memzone<Address, T>, const U &value) {
  return write(upd::two_complement, id, memzone<Address, T>{}, value);
}

} // namespace v2
} // namespace ldp

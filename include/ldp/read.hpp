//! \file
//! \brief Write instruction utilities

#pragma once

#include <cstdint>

#include <upd/format.hpp>

#include "memzone.hpp"
#include "packet.hpp"
#include "request.hpp"
#include "ticket.hpp"

namespace ldp {
inline namespace v2 {
//! \brief Represents the value returned by a device after a read request
//! \tparam T Type of the value
template <typename T> struct device_data {
  packet_id id;
  T value;
};

//! \brief Request class for a read instruction
//! \tparam Signed_Mode Signed integer representation of the packets involved in the request
//! \tparam T Type of the value to be read
template <upd::signed_mode Signed_Mode, typename T>
using read_t = request<Signed_Mode, ticket<Signed_Mode, device_data<T>, T>, address_t, std::uint16_t>;

//! \brief Prepare the content of a read instruction packet
//! \tparam Signed_Mode Signed integer representation of the packet
//! \tparam Address Start of the memory zone to read on
//! \tparam T Type associated with the memory zone
//! \param id Identifier of the target device
//! \return A request object that holds the necessary data for a read instruction
template <upd::signed_mode Signed_Mode, address_t Address, typename T>
read_t<Signed_Mode, T> read(upd::signed_mode_h<Signed_Mode>, packet_id id, memzone<Address, T>) {
  return read_t<Signed_Mode, T>{id, instruction::READ, Address, sizeof(T)};
}

//! \copybrief read
//! \tparam Address Start of the memory zone to read on
//! \tparam T Type associated with the memory zone
//! \param id Identifier of the target device
//! \return A request object that holds the necessary data for a read instruction
template <address_t Address, typename T>
read_t<upd::signed_mode::TWO_COMPLEMENT, T> read(packet_id id, memzone<Address, T>) {
  return read(upd::two_complement, id, memzone<Address, T>{});
}
} // namespace v2
} // namespace ldp

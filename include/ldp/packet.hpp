//! \file
//! \brief Basic packet handling
#pragma once

#include <algorithm>
#include <cstdint>
#include <iterator>

#include <tl/expected.hpp>
#include <upd/format.hpp>
#include <upd/tuple.hpp>
#include <upd/type.hpp>

#include "detail/def.hpp"
#include "detail/packet.hpp"
#include "sentry.hpp"

namespace ldp {
inline namespace v2 {
namespace detail {
//! \brief Calculate the value of the field 'Length' in a packet
template <typename It> length_t calculate_length(It begin, It end) {
  return std::distance(begin, end) + std::count_if(begin, end, stuffing_sentry{}) + sizeof(instruction_t) +
         sizeof(crc_t);
}
} // namespace detail

//! \brief Type for the field 'Packet ID' in packets
using packet_id = uint8_t;

//! \brief Enumeration of the different values for the 'Instruction' field
enum class instruction : detail::instruction_t {
  PING = 0x1,
  READ = 0x2,
  WRITE = 0x3,
  REG_WRITE = 0x4,
  ACTION = 0x5,
  FACTORY_RESET = 0x6,
  REBOOT = 0x8,
  CLEAR = 0x10,
  CONTROL_TABLE_BACKUP = 0x20,
  RETURN = 0x55,
  SYNC_READ = 0x82,
  SYNC_WRITE = 0x83,
  FAST_SYNC_READ = 0x8a,
  BULK_READ = 0x92,
  BULK_WRITE = 0x93,
  FAST_BULK_READ = 0x9a
};

//! \brief Used to indicate the status of an operation
//! \details
//!   It aggregates the library status code and the DYNAMIXEL Protocol 2.0 error code.
//!   The alert flag behaves as defined by the DYNAMIXEL Protocol 2.0.
struct error {
  enum type_t : detail::error_t {
    OK = 0x0,
    RESULT_FAIL = 0x1,
    INSTRUCTION = 0x2,
    CRC = 0x3,
    DATA_RANGE = 0x4,
    DATA_LENGTH = 0x5,
    DATA_LIMIT = 0x6,
    ACCESS = 0x7,
    NOT_STATUS = 0xff,
    RECEIVED_BAD_CRC = 0xfe
  };

  type_t type;
  bool alert;

  error(int type, int alert = false) : type{static_cast<type_t>(type)}, alert(alert) {}
};

//! \brief Write a packet using the provided output functor
//! \param dest_ftor Functor called each time the function must send a byte
//! \param signed_mode Signed number representation in the packet
//! \param id Value of the field 'Packet ID'
//! \param ins Value of the field 'Instruction'
//! \param parameters Values of the field 'Param'
template <typename F, upd::signed_mode Signed_Mode, typename It>
void write_packet(F &&dest_ftor, upd::signed_mode_h<Signed_Mode> signed_mode, packet_id id, instruction ins,
                  It parameters_begin, It parameters_end) {
  using namespace detail;

  auto packet = upd::make_tuple(upd::little_endian, signed_mode, header, id,
                                calculate_length(parameters_begin, parameters_end), static_cast<instruction_t>(ins));
  crc_t crc = 0;

  auto write = [&](upd::byte_t byte) {
    dest_ftor(byte);
    advance_crc(crc, byte);
  };

  for (auto byte : upd::make_view<0, 4>(packet))
    write(byte);

  stuffing_sentry s;
  for (auto it = parameters_begin; it != parameters_end; ++it) {
    auto byte = *it;
    if (s(byte))
      write(stuffing_byte);
    write(byte);
  }

  for (auto byte : upd::make_tuple(upd::little_endian, signed_mode, crc))
    dest_ftor(byte);
}

//! \brief Read a packet content (without header) from an input functor
//! \param src_ftor functor called each time a new byte of the packet must be read
//! \param signed_mode signed number representation in the packet
//! \param parameters_it output iterator to write the byte sequence describing the parameters
//! \return the identifier of the received packet on success, otherwise :
//!   - error::NOT_STATUS if the packet instruction field does not denote a status packet (in that case, parameters are
//!   not output)
//!   - error::RECEIVED_BAD_CRC if the packet CRC is incorrect
//!   - the error field of the packet if it does not indicate a success
template <typename F, upd::signed_mode Signed_Mode, typename It>
tl::expected<packet_id, error> read_headerless_packet(F &&src_ftor, upd::signed_mode_h<Signed_Mode> signed_mode,
                                                      It parameters_it) {
  using namespace detail;

  auto metadata = upd::make_tuple<packet_id, length_t, instruction_t, error_t>(upd::little_endian, signed_mode);
  crc_t crc = 0;
  advance_crc(crc, header);

  auto read = [&]() {
    auto byte = src_ftor();
    advance_crc(crc, byte);
    return byte;
  };

  for (auto &byte : metadata)
    byte = read();
  auto id = upd::get<0>(metadata);
  auto length = upd::get<1>(metadata) - sizeof(instruction_t) - sizeof(crc_t) - sizeof(error_t);
  auto ins = upd::get<2>(metadata);
  auto err = upd::get<3>(metadata);

  ASSERT(ins != status_byte, error::NOT_STATUS);

  stuffing_sentry s;
  for (; length != 0; ++parameters_it, --length) {
    auto byte = read();
    if (s(byte))
      read();
    *parameters_it = byte;
  }

  for (auto byte : upd::make_tuple(upd::little_endian, signed_mode, crc))
    ASSERT(byte != src_ftor(), error::RECEIVED_BAD_CRC);
  ASSERT(err != static_cast<error_t>(error::OK), (error{err & ~alert_bm, err & alert_bm}));

  return id;
}

} // namespace v2
} // namespace ldp

#include "detail/undef.hpp" // IWYU pragma: keep

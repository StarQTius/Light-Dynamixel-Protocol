//! \file
//! \brief Basic packet handling
#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <tl/expected.hpp>
#include <upd/format.hpp>
#include <upd/tuple.hpp>
#include <upd/type.hpp>

#include "detail/def.hpp"

namespace ldp {
inline namespace v2 {
namespace detail {

//! \brief Type for the field 'Length' in packets
using length_t = uint16_t;

//! \brief Type for the field 'Instruction' in packets
using instruction_t = uint8_t;

//! \brief Type for the field 'Error' in packets
using error_t = uint8_t;

//! \brief Type for the field 'CRC' in packets
using crc_t = uint16_t;

//! \brief Value of the field 'Header' in packets
constexpr upd::byte_t header[] = {0xff, 0xff, 0xfd, 0x0};

//! \brief Byte added during packet byte stuffing
constexpr upd::byte_t stuffing_byte = 0xfd;

//! \brief Byte denoting a status packet
constexpr upd::byte_t status_byte = 0x55;

//! \brief Bit mask for the alert flag
constexpr upd::byte_t alert_bm = 1 << 7;

//! \brief Table used during CRC calculation
constexpr crc_t crc_table[] = {
    0x0000, 0x8005, 0x800f, 0x000a, 0x801b, 0x001e, 0x0014, 0x8011, 0x8033, 0x0036, 0x003c, 0x8039, 0x0028, 0x802d,
    0x8027, 0x0022, 0x8063, 0x0066, 0x006c, 0x8069, 0x0078, 0x807d, 0x8077, 0x0072, 0x0050, 0x8055, 0x805f, 0x005a,
    0x804b, 0x004e, 0x0044, 0x8041, 0x80c3, 0x00c6, 0x00cc, 0x80c9, 0x00d8, 0x80dd, 0x80d7, 0x00d2, 0x00f0, 0x80f5,
    0x80ff, 0x00fa, 0x80eb, 0x00ee, 0x00e4, 0x80e1, 0x00a0, 0x80a5, 0x80af, 0x00aa, 0x80bb, 0x00be, 0x00b4, 0x80b1,
    0x8093, 0x0096, 0x009c, 0x8099, 0x0088, 0x808d, 0x8087, 0x0082, 0x8183, 0x0186, 0x018c, 0x8189, 0x0198, 0x819d,
    0x8197, 0x0192, 0x01b0, 0x81b5, 0x81bf, 0x01ba, 0x81ab, 0x01ae, 0x01a4, 0x81a1, 0x01e0, 0x81e5, 0x81ef, 0x01ea,
    0x81fb, 0x01fe, 0x01f4, 0x81f1, 0x81d3, 0x01d6, 0x01dc, 0x81d9, 0x01c8, 0x81cd, 0x81c7, 0x01c2, 0x0140, 0x8145,
    0x814f, 0x014a, 0x815b, 0x015e, 0x0154, 0x8151, 0x8173, 0x0176, 0x017c, 0x8179, 0x0168, 0x816d, 0x8167, 0x0162,
    0x8123, 0x0126, 0x012c, 0x8129, 0x0138, 0x813d, 0x8137, 0x0132, 0x0110, 0x8115, 0x811f, 0x011a, 0x810b, 0x010e,
    0x0104, 0x8101, 0x8303, 0x0306, 0x030c, 0x8309, 0x0318, 0x831d, 0x8317, 0x0312, 0x0330, 0x8335, 0x833f, 0x033a,
    0x832b, 0x032e, 0x0324, 0x8321, 0x0360, 0x8365, 0x836f, 0x036a, 0x837b, 0x037e, 0x0374, 0x8371, 0x8353, 0x0356,
    0x035c, 0x8359, 0x0348, 0x834d, 0x8347, 0x0342, 0x03c0, 0x83c5, 0x83cf, 0x03ca, 0x83db, 0x03de, 0x03d4, 0x83d1,
    0x83f3, 0x03f6, 0x03fc, 0x83f9, 0x03e8, 0x83ed, 0x83e7, 0x03e2, 0x83a3, 0x03a6, 0x03ac, 0x83a9, 0x03b8, 0x83bd,
    0x83b7, 0x03b2, 0x0390, 0x8395, 0x839f, 0x039a, 0x838b, 0x038e, 0x0384, 0x8381, 0x0280, 0x8285, 0x828f, 0x028a,
    0x829b, 0x029e, 0x0294, 0x8291, 0x82b3, 0x02b6, 0x02bc, 0x82b9, 0x02a8, 0x82ad, 0x82a7, 0x02a2, 0x82e3, 0x02e6,
    0x02ec, 0x82e9, 0x02f8, 0x82fd, 0x82f7, 0x02f2, 0x02d0, 0x82d5, 0x82df, 0x02da, 0x82cb, 0x02ce, 0x02c4, 0x82c1,
    0x8243, 0x0246, 0x024c, 0x8249, 0x0258, 0x825d, 0x8257, 0x0252, 0x0270, 0x8275, 0x827f, 0x027a, 0x826b, 0x026e,
    0x0264, 0x8261, 0x0220, 0x8225, 0x822f, 0x022a, 0x823b, 0x023e, 0x0234, 0x8231, 0x8213, 0x0216, 0x021c, 0x8219,
    0x0208, 0x820d, 0x8207, 0x0202};

//! \brief Compute the next CRC value according the provided byte sequence
void advance_crc(crc_t &crc, upd::byte_t byte) { crc = (crc << 8u) ^ crc_table[((crc >> 8u) ^ byte) & 0xff]; }
template <typename R> void advance_crc(crc_t &crc, R &&seq) {
  for (auto byte : FWD(seq))
    advance_crc(crc, byte);
}

//! \brief Counts up the element of a byte sequence and indicates if the next byte must be a stuffing byte
struct sentry {
  std::size_t count;

  sentry() : count{0} {}

  bool operator()(upd::byte_t byte) {
    count = header[count] == byte ? count + 1 : (header[0] == byte ? 1 : 0);
    if (count == sizeof header - 1) {
      count = 0;
      return true;
    } else {
      return false;
    }
  }
};

//! \brief Calculate the value of the field 'Length' in a packet
template <typename It> length_t calculate_length(It begin, It end) {
  length_t stuffed_length = 0;
  size_t stuff_sentry = 0;

  for (auto it = begin; it != end; ++it, ++stuffed_length) {
    auto byte = *it;
    stuff_sentry = header[stuff_sentry] == byte ? stuff_sentry + 1 : (header[0] == byte ? 1 : 0);

    if (stuff_sentry == sizeof header - 1) {
      stuffed_length++;
      stuff_sentry = 0;
    }
  }

  return stuffed_length + sizeof(instruction_t) + sizeof(crc_t);
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

  sentry s;
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

  if (ins != status_byte)
    return tl::make_unexpected(error::NOT_STATUS);

  sentry s;
  for (; length != 0; ++parameters_it, --length) {
    auto byte = read();
    if (s(byte))
      read();
    *parameters_it = byte;
  }

  for (auto byte : upd::make_tuple(upd::little_endian, signed_mode, crc))
    if (byte != src_ftor())
      return tl::make_unexpected(error::RECEIVED_BAD_CRC);

  if (err != static_cast<error_t>(error::OK))
    return tl::make_unexpected(error{err & ~alert_bm, err & alert_bm});

  return id;
}

} // namespace v2
} // namespace ldp

#include "detail/undef.hpp" // IWYU pragma: keep

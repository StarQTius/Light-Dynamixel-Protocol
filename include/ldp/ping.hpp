//! \file
//! \brief Ping instruction utilities

#include <cstdint>

#include <upd/format.hpp>

#include "packet.hpp"
#include "request.hpp"
#include "ticket.hpp"

namespace ldp {
inline namespace v2 {

//! \brief Reponse from a device to a ping instruction
//! \details
//!   https://emanual.robotis.com/docs/en/dxl/protocol2/#packet-parameters
struct device_info {
  //! \brief Identifier of the device stored in its memory
  packet_id id;

  //! \brief Model of the device
  std::uint16_t model_number;

  //! \brief Version of the device firmware
  std::uint8_t firmware_version;
};

//! \brief Request class for a ping instruction
//! \tparam Signed_Mode Signed integer representation of the packets involved in the request
template <upd::signed_mode Signed_Mode>
using ping_t = request<Signed_Mode, ticket<Signed_Mode, device_info, std::uint16_t, std::uint8_t>>;

//! \brief Prepare the content of a ping instruction packet
//! \tparam Signed_Mode Signed integer representation of the packet
//! \param id Identifier of the target device
//! \return A request object that holds the necessary data for a ping instruction
template <upd::signed_mode Signed_Mode> ping_t<Signed_Mode> ping(upd::signed_mode_h<Signed_Mode>, packet_id id) {
  return ping_t<Signed_Mode>{id, instruction::PING};
}

//! \copybrief ping
//! \param id Identifier of the target device
//! \return A request object that holds the necessary data for a ping instruction
ping_t<upd::signed_mode::TWO_COMPLEMENT> ping(packet_id id) { return ping(upd::two_complement, id); }

//! \copybrief ping
//! \details
//!   The ping request will be broadcast to every devices in the bus it will be sent in
//! \tparam Signed_Mode Signed integer representation of the packet
//! \return A request object that holds the necessary data for a ping instruction
template <upd::signed_mode Signed_Mode> ping_t<Signed_Mode> ping(upd::signed_mode_h<Signed_Mode> smode) {
  return ping(smode, broadcast);
}

//! \copybrief ping
//! \details
//!   The ping request will be broadcast to every devices in the bus it will be sent in
//! \return A request object that holds the necessary data for a ping instruction
ping_t<upd::signed_mode::TWO_COMPLEMENT> ping() { return ping(upd::two_complement, broadcast); }

} // namespace v2
} // namespace ldp

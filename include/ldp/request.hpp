//! \file
//! \brief Bus control

#pragma once

#include <tl/expected.hpp>
#include <upd/format.hpp>
#include <upd/tuple.hpp>
#include <upd/type.hpp>

#include "detail/sfinae.hpp"
#include "packet.hpp"

#include "detail/def.hpp"

namespace ldp {
inline namespace v2 {

//! \brief Process packets following a sent request
//! \tparam Signed_Mode Signed integer convention of the received packet
//! \tparam T Type of the value extracted from the packets
//! \tparam Ts Type mapping of the 'Parameters' field
template <upd::signed_mode Signed_Mode, typename T, typename... Ts> class ticket {
public:
  //! \brief Extract the value from a packet
  //! \details
  //!   Each byte of the packet is delivered by the provided functor.
  //! \param ftor Functor which delivers a byte each time it is called
  template <typename F, sfinae::require_input_ftor<F> = 0> tl::expected<T, error> operator<<(F &&ftor) const {
    upd::tuple<upd::endianess::LITTLE, Signed_Mode, Ts...> parameters;
    auto maybe_id =
        read_headerless_packet(FWD(ftor), upd::signed_mode_h<Signed_Mode>{}, parameters.begin(), parameters.end());

    auto make_value = [&](packet_id id) { return parameters.invoke([&](const Ts &...xs) { return T{id, xs...}; }); };
    return maybe_id.map(make_value);
  }

  //! \copybrief operator<<
  //! \details
  //!   Each byte of the packet is delivered by the provided iterator.
  //! \param it Start of the packet
  template <typename It, sfinae::require_is_iterator<It> = 0> tl::expected<T, error> operator<<(It it) const {
    return operator<<([&]() { return *it++; });
  }
};

//! \brief Make a request object which holds the necessary data to send an instruction packet
template <upd::signed_mode Signed_Mode, typename Tk, typename... Ts> class request {
public:
  //! \brief Store explicitly the values of the instruction packet field
  //! \param id Target device identifier
  //! \param ins Instruction to the target device
  //! \param parameters Parameters of the instruction
  explicit request(packet_id id, instruction ins, const Ts &...parameters)
      : m_id{id}, m_ins{ins}, m_parameters{parameters...} {}

  //! \brief Send the packet
  //! \details
  //!   Each byte of the packet will be called on a provided functor.
  //! \param ftor Functor which will send a byte each time it is called
  template <typename F, sfinae::require_output_ftor<F> = 0> Tk operator>>(F &&ftor) const {
    write_packet(FWD(ftor), upd::signed_mode_h<Signed_Mode>{}, m_id, m_ins, m_parameters.begin(), m_parameters.end());
    return {};
  }

  //! \copybrief operator>>
  //! \details
  //!   Each byte will be written using the provided iterator
  //! \param it Start of the byte sequence to write to
  template <typename It, sfinae::require_is_iterator<It> = 0> Tk operator>>(It it) const {
    return operator>>([&](upd::byte_t byte) { *it++ = byte; });
  }

private:
  packet_id m_id;
  instruction m_ins;
  upd::tuple<upd::endianess::LITTLE, Signed_Mode, Ts...> m_parameters;
};

} // namespace v2
} // namespace ldp

#include "detail/undef.hpp" // IWYU pragma: keep

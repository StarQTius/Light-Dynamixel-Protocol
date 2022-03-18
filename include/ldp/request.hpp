//! \file
//! \brief Bus control

#pragma once

#include <upd/format.hpp>
#include <upd/tuple.hpp>
#include <upd/type.hpp>

#include "detail/sfinae.hpp"
#include "packet.hpp"

#include "detail/def.hpp"

namespace ldp {
inline namespace v2 {
namespace detail {

//! \brief Base class defining members for request classes using CRTP
//! \details Deriving classes must define a 'write' function member that accepts a functor and returns a ticket.
template <typename D, typename Tk> class request_base {
  D &derived() { return reinterpret_cast<D &>(*this); }
  const D &derived() const { return reinterpret_cast<const D &>(*this); }

public:
  //! \brief Send the packet
  //! \details
  //!   Each byte of the packet will be called on a provided functor.
  //! \param ftor Functor which will send a byte each time it is called
  template <typename F, sfinae::require_output_ftor<F> = 0> Tk operator>>(F &&ftor) {
    return derived().write(FWD(ftor));
  }

  //! \copybrief operator>>
  //! \details
  //!   Each byte will be written using the provided iterator
  //! \param it Start of the byte sequence to write to
  template <typename It, sfinae::require_is_iterator<It> = 0> Tk operator>>(It it) {
    return derived().write([&](upd::byte_t byte) { *it++ = byte; });
  }
};

} // namespace detail

//! \brief Make a request object which holds the necessary data to send an instruction packet
template <upd::signed_mode Signed_Mode, typename Tk, typename... Ts>
class request : public detail::request_base<request<Signed_Mode, Tk, Ts...>, Tk> {
public:
  //! \brief Store explicitly the values of the instruction packet field
  //! \param id Target device identifier
  //! \param ins Instruction to the target device
  //! \param parameters Parameters of the instruction
  explicit request(packet_id id, instruction ins, const Ts &...parameters)
      : m_id{id}, m_ins{ins}, m_parameters{parameters...} {}

  //! \brief Call a functor on each byte of the packet
  //! \param ftor The functor to call
  //! \return A ticket that can interpret the response from the target device
  template <typename F, sfinae::require_output_ftor<F> = 0> Tk write(F &&ftor) const {
    write_packet(FWD(ftor), upd::signed_mode_h<Signed_Mode>{}, m_id, m_ins, m_parameters.begin(), m_parameters.end());
    return {};
  }

private:
  packet_id m_id;
  instruction m_ins;
  upd::tuple<upd::endianess::LITTLE, Signed_Mode, Ts...> m_parameters;
};

} // namespace v2
} // namespace ldp

#include "detail/undef.hpp" // IWYU pragma: keep

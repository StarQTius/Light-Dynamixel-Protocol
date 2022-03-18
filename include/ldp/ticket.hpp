//! \file
//! \brief Device response handling

#pragma once

#include <tl/expected.hpp>
#include <upd/format.hpp>
#include <upd/tuple.hpp>

#include "detail/any_function.hpp"
#include "detail/sfinae.hpp"
#include "packet.hpp"

#include "detail/def.hpp"

namespace ldp {
inline namespace v2 {

//! \brief Stores a callback to be called on the response from a device
//! \details This class is non-templated, therefore it is suitable for storage.
class ticket_with_hook {
  template <upd::signed_mode, typename, typename...> friend class ticket;

public:
  //! \brief Call the stored callback on the provided parameters
  //! \param input_ftor Input functor the parameters will be extracted from
  //! \return The error code resulting from the call to 'read_headerless_packet'
  template <typename F, sfinae::require_input_ftor<F> = 0> error operator()(F &&input_ftor) const {
    return m_restorer(m_callback_ptr, detail::abstract_input_functor<F>{input_ftor});
  }

  //! \copybrief operator()
  //! \param it Start of the range the parameters will be extracted from
  //! \return The error code resulting from the call to 'read_headerless_packet'
  template <typename It, sfinae::require_is_iterator<It> = 0> error operator()(It it) const {
    return operator()([&]() { return *it++; });
  }

private:
  //! \brief Store a functor convertible to function pointer
  template <typename F, typename Tk> explicit ticket_with_hook(F &&ftor, Tk) : ticket_with_hook{+ftor, Tk{}} {}

  //! \brief Store a function pointer as callback and its restorer
  template <typename R, typename... Args, typename Tk>
  explicit ticket_with_hook(R (*f_ptr)(Args...), Tk)
      : m_callback_ptr{reinterpret_cast<detail::any_function_t *>(f_ptr)},
        m_restorer{detail::restore_and_call<R(Args...), Tk>} {}

  detail::any_function_t *m_callback_ptr;
  detail::restorer_t *m_restorer;
};

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

  //! \brief Hook a callback to the ticket
  //! \param hook The callback to store
  //! \return A ticket with the provided hook
  template <typename F> ticket_with_hook with_hook(F &&hook) { return ticket_with_hook{FWD(hook), *this}; }
};

} // namespace v2
} // namespace ldp

#include "detail/undef.hpp" // IWYU pragma: keep

//! \file
//! \brief Private macro utilities

#define FWD(X) static_cast<decltype(X) &&>(X)

//! \file
//! \brief Private macro utilities

#define FWD(X) static_cast<decltype(X) &&>(X)
#define ASSERT(EXPR, ERROR)                                                                                            \
  if (EXPR)                                                                                                            \
    return ::tl::make_unexpected(ERROR);

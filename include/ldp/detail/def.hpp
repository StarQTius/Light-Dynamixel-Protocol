//! \file
//! \brief Private macro utilities

#define FWD(X) static_cast<decltype(X) &&>(X)
#define PACK(...) __VA_ARGS__
#define ASSERT(EXPR, ERROR)                                                                                            \
  if (EXPR)                                                                                                            \
    return ::tl::make_unexpected(ERROR);

#pragma once

#include <unity.h>

extern "C" void setUp() {}
extern "C" void tearDown() {}

#ifdef FWD
#error "Private macros have leaked"
#endif // FWD

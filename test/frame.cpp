#include <dxl/frame.hpp>

#include <unity.h>

#ifdef FWD
#error "Private macros have leaked"
#endif // FWD

static void frame_DO_send_a_frame() {
  using namespace dxl;

  upd::byte_t buf[64], *ptr = buf;
  auto tuple = upd::make_tuple(uint16_t{132}, uint16_t{4});
  auto write = [&](upd::byte_t byte) { *ptr++ = byte; };

  write_frame(write, upd::two_complement, 0x1, instruction::READ, tuple.begin(), tuple.end());

  constexpr upd::byte_t expected[] = {0xff, 0xff, 0xfd, 0x0, 0x1, 0x7, 0x0, 0x2, 0x84, 0x0, 0x4, 0x0, 0x1d, 0x15};
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, buf, sizeof expected);
}

extern "C" void setUp() {}
extern "C" void tearDown() {}

int main() { RUN_TEST(frame_DO_send_a_frame); }

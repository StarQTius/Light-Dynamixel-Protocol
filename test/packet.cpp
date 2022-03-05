#include <dxl/packet.hpp>

#include <unity.h>

#ifdef FWD
#error "Private macros have leaked"
#endif // FWD

static void packet_DO_send_a_packet() {
  using namespace dxl;

  upd::byte_t buf[64], *ptr = buf;
  auto tuple = upd::make_tuple(uint16_t{132}, uint16_t{4});
  auto write = [&](upd::byte_t byte) { *ptr++ = byte; };

  write_packet(write, upd::two_complement, 0x1, instruction::READ, tuple.begin(), tuple.end());

  constexpr upd::byte_t expected[] = {0xff, 0xff, 0xfd, 0x0, 0x1, 0x7, 0x0, 0x2, 0x84, 0x0, 0x4, 0x0, 0x1d, 0x15};
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, buf, sizeof expected);
}

static void packet_DO_receive_a_headerless_packet() {
  using namespace dxl;

  constexpr upd::byte_t input[] = {0xff, 0xff, 0xfd, 0x00, 0x01, 0x08, 0x00, 0x55,
                                   0x00, 0xa6, 0x00, 0x00, 0x00, 0x8c, 0xc0};
  upd::byte_t output[5] = {};
  const upd::byte_t *ptr = input + 4;
  auto read = [&]() { return *ptr++; };

  read_headerless_packet(read, upd::two_complement, output)
      .map([&](packet_id id) { TEST_ASSERT_EQUAL_HEX8(id, 0x1); })
      .map_error([&](error) { TEST_FAIL(); });

  TEST_ASSERT_EQUAL_HEX8(0x0, output[4]);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(input + 9, output, 4);
}

extern "C" void setUp() {}
extern "C" void tearDown() {}

int main() {
  UNITY_BEGIN();
  RUN_TEST(packet_DO_send_a_packet);
  RUN_TEST(packet_DO_receive_a_headerless_packet);
  return UNITY_END();
}

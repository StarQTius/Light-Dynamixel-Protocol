#include <list>
#include <vector>

#include <ldp/ping.hpp>

#include "utility.hpp"

struct mock_bus {
  mock_bus(const std::vector<upd::byte_t> &expected, const std::vector<upd::byte_t> &answer)
      : expected{expected}, answer{answer}, buf{64} {}

  std::vector<upd::byte_t> expected, answer;
  std::vector<upd::byte_t> buf;

  void shift() {
    if (std::equal(expected.begin(), expected.end(), buf.begin()))
      buf = answer;
  }
};

void request_DO_send_a_ping_request() {
  using namespace ldp;

  mock_bus mb{{0xff, 0xff, 0xfd, 0x00, 0x01, 0x03, 0x00, 0x01, 0x19, 0x4e},
              {0x01, 0x07, 0x00, 0x55, 0x00, 0x06, 0x04, 0x26, 0x65, 0x5d}};

  auto t = ping(0x01) >> mb.buf.begin();
  mb.shift();
  auto response = t << mb.buf.begin();
  response
      .map([](const device_info &info) {
        TEST_ASSERT_EQUAL(info.id, 0x01);
        TEST_ASSERT_EQUAL(info.model_number, 1030);
        TEST_ASSERT_EQUAL(info.firmware_version, 38);
      })
      .or_else([](error) { TEST_FAIL(); });
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(request_DO_send_a_ping_request);
  return UNITY_END();
}

#include <vector>

#include <ldp/ping.hpp>
#include <ldp/read.hpp>
#include <ldp/write.hpp>

#include "utility.hpp"

struct mock_bus {
  mock_bus(const std::vector<upd::byte_t> &expected, const std::vector<upd::byte_t> &answer)
      : expected{expected}, answer{answer}, buf(64) {}

  std::vector<upd::byte_t> expected, answer;
  std::vector<upd::byte_t> buf;

  void shift() {
    if (std::equal(expected.begin(), expected.end(), buf.begin())) {
      buf = answer;
    } else {
      TEST_FAIL_MESSAGE("Packet mismatch");
    }
  }
};

static void request_DO_send_a_request_with_hook() {
  using namespace ldp;

  mock_bus mb{{0xff, 0xff, 0xfd, 0x00, 0x01, 0x03, 0x00, 0x01, 0x19, 0x4e},
              {0x01, 0x07, 0x00, 0x55, 0x00, 0x06, 0x04, 0x26, 0x65, 0x5d}};

  auto t = ping(0x01) >> mb.buf.begin();
  auto t_wh = t.with_hook([](const device_info &info) {
    TEST_ASSERT_EQUAL(info.id, 0x01);
    TEST_ASSERT_EQUAL(info.model_number, 1030);
    TEST_ASSERT_EQUAL(info.firmware_version, 38);
  });
  mb.shift();
  TEST_ASSERT_EQUAL(error::OK, t_wh(mb.buf.begin()).type);
}

static void request_DO_send_a_ping_request() {
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

static void request_DO_send_a_write_request() {
  using namespace ldp;

  mock_bus mb{{0xff, 0xff, 0xfd, 0x00, 0x01, 0x09, 0x00, 0x03, 0x74, 0x00, 0x00, 0x02, 0x00, 0x00, 0xca, 0x89},
              {0x01, 0x04, 0x00, 0x55, 0x00, 0xa1, 0x0c}};

  auto t = write(0x01, memzone<116, uint32_t>{}, 512) >> mb.buf.begin();
  mb.shift();
  auto response = t << mb.buf.begin();
  response.map([](packet_id id) { TEST_ASSERT_EQUAL(0x01, id); }).or_else([](error) { TEST_FAIL(); });
}

static void request_DO_send_a_read_request() {
  using namespace ldp;

  mock_bus mb{{0xff, 0xff, 0xfd, 0x00, 0x01, 0x07, 0x00, 0x02, 0x84, 0x00, 0x04, 0x00, 0x1d, 0x15},
              {0x01, 0x08, 0x00, 0x55, 0x00, 0xa6, 0x00, 0x00, 0x00, 0x8c, 0xc0}};

  auto t = read(0x01, memzone<132, uint32_t>{}) >> mb.buf.begin();
  mb.shift();
  auto response = t << mb.buf.begin();
  response
      .map([](device_data<uint32_t> data) {
        TEST_ASSERT_EQUAL(0x01, data.id);
        TEST_ASSERT_EQUAL_UINT32(166, data.value);
      })
      .or_else([](error) { TEST_FAIL(); });
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(request_DO_send_a_request_with_hook);
  RUN_TEST(request_DO_send_a_ping_request);
  RUN_TEST(request_DO_send_a_write_request);
  RUN_TEST(request_DO_send_a_read_request);
  return UNITY_END();
}

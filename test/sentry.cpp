#include <ldp/sentry.hpp>

#include "utility.hpp"

static void sentry_DO_detect_a_header() {
  using namespace ldp;

  constexpr upd::byte_t seq[] = {0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xfd, 0x00, 0xff, 0xfd, 0xff};

  sentry s;
  std::size_t i = 0;
  for (; i < sizeof seq; ++i) {
    if (s(seq[i]))
      break;
  }

  TEST_ASSERT_EQUAL(7, i);
}

static void sentry_DO_detect_multiple_headers() {
  using namespace ldp;

  constexpr upd::byte_t seq[] = {0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xff, 0xfd, 0x00,
                                 0xff, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x00, 0xff, 0xff, 0xff, 0xfd};

  sentry s;
  std::size_t result[16] = {}, expected[] = {11, 19}, *p = result;
  for (std::size_t i = 0; i < sizeof seq; ++i) {
    if (s(seq[i]))
      *p++ = i;
  }

  TEST_ASSERT_EQUAL(p - result, 2);
  TEST_ASSERT_EQUAL_INT_ARRAY(expected, result, sizeof expected / sizeof *expected);
}

static void sentry_DO_detect_a_stuffing_byte() {
  using namespace ldp;

  constexpr upd::byte_t seq[] = {0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xfd, 0xfd, 0xff, 0xfd, 0xff};

  stuffing_sentry s;
  std::size_t i = 0;
  for (; i < sizeof seq; ++i) {
    if (s(seq[i]))
      break;
  }

  TEST_ASSERT_EQUAL(6, i);
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(sentry_DO_detect_a_header);
  RUN_TEST(sentry_DO_detect_multiple_headers);
  RUN_TEST(sentry_DO_detect_a_stuffing_byte);
  return UNITY_END();
}

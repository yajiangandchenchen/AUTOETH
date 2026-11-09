// AUTOETH lightweight assertion helper (D-2: no third-party test framework).
#ifndef AUTOETH_TEST_CHECK_HPP
#define AUTOETH_TEST_CHECK_HPP

#include <cstdio>

namespace autoeth_test {

struct Counters {
  int passed;
  int failed;

  Counters() noexcept : passed(0), failed(0) {}
};

inline Counters& Global() noexcept {
  static Counters counters;
  return counters;
}

inline void Record(bool condition, const char* expression, const char* file, int line) {
  if (condition) {
    ++Global().passed;
  } else {
    ++Global().failed;
    std::fprintf(stderr, "FAIL %s:%d  %s\n", file, line, expression);
  }
}

inline int Summary(const char* suite) {
  std::printf("%s: %d passed, %d failed\n", suite, Global().passed, Global().failed);
  return (Global().failed == 0) ? 0 : 1;
}

}  // namespace autoeth_test

#define AUTOETH_CHECK(expression) \
  ::autoeth_test::Record((expression), #expression, __FILE__, __LINE__)

#endif
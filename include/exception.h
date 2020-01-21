#ifndef LOX_EXCEPTION_H
#define LOX_EXCEPTION_H

#include <stdexcept>

#include "types.h"

namespace lox {

struct internal_error : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct scan_error : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct parse_error : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct runtime_error : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct assert_exception : std::runtime_error {
  using std::runtime_error::runtime_error;
};

#ifdef NDEBUG

#define Expect(condition, message) ((void)0)

#else

inline void on_expect_fail(const string& message, const char* file, int line) {
  throw assert_exception{"Expect: " + string{file} + " (" +
                         std::to_string(line) + ") " + message};
}

#define Expect(condition, message) \
  ((condition) ? ((void)0) : lox::on_expect_fail(message, __FILE__, __LINE__))

#endif

}  // namespace lox

#endif
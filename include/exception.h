#ifndef LOX_EXCEPTION_H
#define LOX_EXCEPTION_H

#include <cassert>
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

#define Expect(condition) assert(condition)
#define Exsure(condition) assert(condition)

}  // namespace lox

#endif
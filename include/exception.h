#ifndef LOX_EXCEPTION_H
#define LOX_EXCEPTION_H

#include <exception>

namespace lox {

struct internal_error : std::runtime_error {
  using runtime_error::runtime_error;
};

struct scan_error : std::runtime_error {
  using runtime_error::runtime_error;
};

}  // namespace lox

#endif

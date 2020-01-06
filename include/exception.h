#ifndef LOX_EXCEPTION_H
#define LOX_EXCEPTION_H

#include <exception>

namespace lox {

struct parse_error : std::runtime_error {
  using runtime_error::runtime_error;
};

struct runtime_error : std::runtime_error {
  using std::runtime_error::runtime_error;
};

}  // namespace lox

#endif
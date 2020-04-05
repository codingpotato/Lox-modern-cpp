#ifndef LOX_EXCEPTION_H
#define LOX_EXCEPTION_H

#include <stdexcept>

namespace lox {

struct exception : std::runtime_error {
  using runtime_error::runtime_error;
};

struct internal_error : exception {
  using exception::exception;
};

struct scan_error : exception {
  using exception::exception;
};

struct compile_error : exception {
  using exception::exception;
};

struct runtime_error : exception {
  using exception::exception;
};

}  // namespace lox

#endif

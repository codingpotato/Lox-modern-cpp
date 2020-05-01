#ifndef LOX_EXCEPTION_H
#define LOX_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace lox {

class Exception : public std::exception {
 public:
  Exception(std::string message) noexcept : message{message} {}

  const char* what() const noexcept override { return message.c_str(); }

 private:
  std::string message;
};

class Internal_error : public Exception {
 public:
  using Exception::Exception;
};

class Compile_error : public Exception {
 public:
  using Exception::Exception;
};

class Runtime_error : public Exception {
 public:
  using Exception::Exception;
};

}  // namespace lox

#endif

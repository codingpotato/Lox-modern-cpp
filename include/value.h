#ifndef LOX_VALUE_H
#define LOX_VALUE_H

#include "exception.h"

namespace lox {

struct value {
  value() noexcept : type{nil} {}
  value(bool b) noexcept : type{boolean}, boolean_value{b} {}
  value(double d) noexcept : type{number}, number_value{d} {}

  template <typename T>
  bool is() const noexcept {
    if constexpr (std::is_same_v<T, bool>) {
      return type == boolean;
    } else if constexpr (std::is_same_v<T, double>) {
      return type == number_value;
    }
  }

  template <typename T>
  const T& as() const {
    if constexpr (std::is_same_v<T, bool>) {
      if (type == boolean) {
        return boolean_value;
      }
      throw runtime_error{"Expect boolean value."};
    } else if constexpr (std::is_same_v<T, double>) {
      if (type == number) {
        return number_value;
      }
      throw runtime_error{"Expect number value."};
    }
  }

 private:
  enum type_t { nil, boolean, number };

  type_t type;
  union {
    bool boolean_value;
    double number_value;
  };
};

}  // namespace lox

#endif

#ifndef LOX_VALUE_H
#define LOX_VALUE_H

#include <string>

#include "exception.h"
#include "object.h"
#include "type_list.h"

namespace lox {

struct nil {};

struct value {
  value() noexcept : type_{nil_t} {}
  value(bool b) noexcept : type_{boolean}, boolean_{b} {}
  value(double d) noexcept : type_{number}, double_{d} {}
  value(object* o) noexcept : type_{object_t}, object_{o} {}

  template <typename T>
  bool is() const noexcept {
    if constexpr (std::is_same_v<T, nil>) {
      return type_ == nil_t;
    } else if constexpr (std::is_same_v<T, bool>) {
      return type_ == boolean;
    } else if constexpr (std::is_same_v<T, double>) {
      return type_ == number;
    } else if constexpr (std::is_same_v<T, object*>) {
      return type_ == object_t;
    }
    return false;
  }

  template <typename T>
  const T& as() const {
    if constexpr (std::is_same_v<T, bool>) {
      return boolean_;
    } else if constexpr (std::is_same_v<T, double>) {
      return double_;
    } else if constexpr (std::is_same_v<T, object*>) {
      return object_;
    }
    throw internal_error{""};
  }

  friend bool operator==(const value& lhs, const value& rhs) noexcept {
    if (lhs.type_ != rhs.type_) {
      return false;
    }
    switch (lhs.type_) {
      case nil_t:
        return true;
      case boolean:
        return lhs.boolean_ == rhs.boolean_;
      case number:
        return lhs.double_ == rhs.double_;
      case object_t:
        return false;
    }
  }

  std::string repr() const {
    switch (type_) {
      case nil_t:
        return "nil";
      case boolean:
        return boolean_ ? "true" : "false";
      case number: {
        std::ostringstream oss;
        oss << double_;
        return oss.str();
      }
      case object_t:
        return "object";
    }
    throw internal_error{""};
  }

 private:
  enum type_t { nil_t, boolean, number, object_t };

  type_t type_;
  union {
    bool boolean_;
    double double_;
    object* object_;
  };
};

inline value operator+(const value& lhs, const value& rhs) {
  return lhs.as<double>() + rhs.as<double>();
}

inline value operator-(const value& lhs, const value& rhs) {
  return lhs.as<double>() - rhs.as<double>();
}

inline value operator*(const value& lhs, const value& rhs) {
  return lhs.as<double>() * rhs.as<double>();
}

inline value operator/(const value& lhs, const value& rhs) {
  return lhs.as<double>() / rhs.as<double>();
}

inline value operator>(const value& lhs, const value& rhs) {
  return lhs.as<double>() > rhs.as<double>();
}

inline value operator<(const value& lhs, const value& rhs) {
  return lhs.as<double>() < rhs.as<double>();
}

}  // namespace lox

#endif

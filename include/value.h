#ifndef LOX_VALUE_H
#define LOX_VALUE_H

#include <memory>
#include <sstream>
#include <string>

#include "exception.h"

namespace lox {

struct nil {};

struct value {
  value() noexcept : type_{type::nil} {}
  value(bool b) noexcept : type_{type::boolean}, boolean_{b} {}
  value(double d) noexcept : type_{type::number}, number_{d} {}
  value(const char* str) noexcept
      : type_{type::string}, string_{std::make_unique<std::string>(str)} {}
  value(std::string str) noexcept
      : type_{type::string},
        string_{std::make_unique<std::string>(std::move(str))} {}

  value(const value& v) noexcept { copy(v, *this); }
  value(value&& v) noexcept { copy(v, *this); }
  value& operator=(const value& v) {
    if (this != &v) {
      copy(v, *this);
    }
    return *this;
  }
  value& operator=(value&& v) {
    if (this != &v) {
      copy(v, *this);
    }
    return *this;
  }

  ~value() noexcept { destory(*this); }

  template <typename T>
  bool is() const noexcept {
    if constexpr (std::is_same_v<T, nil>) {
      return type_ == type::nil;
    } else if constexpr (std::is_same_v<T, bool>) {
      return type_ == type::boolean;
    } else if constexpr (std::is_same_v<T, double>) {
      return type_ == type::number;
    } else if constexpr (std::is_same_v<T, std::string>) {
      return type_ == type::string;
    }
  }

  template <typename T>
  const T& as() const {
    if constexpr (std::is_same_v<T, bool>) {
      if (type_ == type::boolean) {
        return boolean_;
      }
      throw runtime_error{"Expect boolean value."};
    } else if constexpr (std::is_same_v<T, double>) {
      if (type_ == type::number) {
        return number_;
      }
      throw runtime_error{"Expect number value."};
    } else if constexpr (std::is_same_v<T, std::string>) {
      if (type_ == type::string) {
        return *string_;
      }
      throw runtime_error{"Expect string value."};
    }
  }

  friend value operator==(const value& lhs, const value& rhs) {
    if (lhs.type_ == rhs.type_) {
      switch (lhs.type_) {
        case type::nil:
          return true;
        case type::boolean:
          return lhs.boolean_ == rhs.boolean_;
        case type::number:
          return lhs.number_ == rhs.number_;
        case type::string:
          return *lhs.string_ == *rhs.string_;
      }
    }
    return false;
  }

  std::string repr() const noexcept {
    switch (type_) {
      case type::nil:
        return "nil";
      case type::boolean:
        return boolean_ ? "true" : "false";
      case type::number: {
        std::ostringstream oss;
        oss << number_;
        return oss.str();
      }
      case type::string:
        return *string_;
    }
    return "";
  }

 private:
  enum class type { nil, boolean, number, string };

  template <typename Value>
  friend inline void copy(Value&& from, value& to) noexcept {
    destory(to);
    to.type_ = from.type_;
    switch (to.type_) {
      case type::nil:
        break;
      case type::boolean:
        to.boolean_ = from.boolean_;
        break;
      case type::number:
        to.number_ = from.number_;
        break;
      case type::string:
        new (&to.string_) std::unique_ptr<std::string>{
            new std::string{*std::forward<Value>(from).string_}};
        break;
    }
  }

  friend inline void destory(const value& v) noexcept {
    if (v.type_ == type::string) {
      v.string_.~unique_ptr();
    }
  }

  type type_;
  union {
    bool boolean_;
    double number_;
    std::unique_ptr<std::string> string_;
  };
};

inline value operator+(const value& lhs, const value& rhs) {
  if (lhs.is<double>() && rhs.as<double>()) {
    return lhs.as<double>() + rhs.as<double>();
  } else if (lhs.is<std::string>() && rhs.is<std::string>()) {
    return lhs.as<std::string>() + rhs.as<std::string>();
  }
  throw runtime_error{"Operands must be two numbers or two strings."};
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

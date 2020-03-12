#ifndef LOX_VALUE_H
#define LOX_VALUE_H

#include <memory>
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

  value(const value& v) noexcept : type_{v.type_} {
    switch (type_) {
      case type::nil:
        break;
      case type::boolean:
        boolean_ = v.boolean_;
        break;
      case type::number:
        number_ = v.number_;
        break;
      case type::string:
        string_ = std::make_unique<std::string>(*v.string_);
    }
  }

  value(value&& v) noexcept : type_{v.type_} {
    switch (type_) {
      case type::nil:
        break;
      case type::boolean:
        boolean_ = v.boolean_;
        break;
      case type::number:
        number_ = v.number_;
        break;
      case type::string:
        string_ = std::move(v.string_);
    }
  }

  value& operator=(const value& v) {
    if (this != &v) {
      type_ = v.type_;
      switch (type_) {
        case type::nil:
          break;
        case type::boolean:
          boolean_ = v.boolean_;
          break;
        case type::number:
          number_ = v.number_;
          break;
        case type::string:
          string_ = std::make_unique<std::string>(*v.string_);
      }
    }
    return *this;
  }

  value& operator=(value&& v) {
    if (this != &v) {
      type_ = v.type_;
      switch (type_) {
        case type::nil:
          break;
        case type::boolean:
          boolean_ = v.boolean_;
          break;
        case type::number:
          number_ = v.number_;
          break;
        case type::string:
          string_ = std::move(v.string_);
      }
    }
    return *this;
  }

  ~value() noexcept {
    if (type_ == type::string) {
      string_.~unique_ptr();
    }
  }

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

 private:
  enum class type { nil, boolean, number, string };

  type type_;
  union {
    bool boolean_;
    double number_;
    std::unique_ptr<std::string> string_;
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

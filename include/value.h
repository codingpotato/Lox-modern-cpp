#ifndef LOX_VALUE_H
#define LOX_VALUE_H

#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>

#include "exception.h"
#include "object.h"
#include "type_list.h"

namespace lox {

struct nil {};

namespace fast {

struct value {
  value() noexcept : type_{nil} {}
  value(bool b) noexcept : type_{boolean}, boolean_{b} {}
  value(double d) noexcept : type_{number}, double_{d} {}
  value(object* o) noexcept : type_{object_t}, object_{o} {}

  template <typename T>
  const T& as() const noexcept {
    (void)type_;
    if constexpr (std::is_same_v<T, bool>) {
      return boolean_;
    } else if constexpr (std::is_same_v<T, double>) {
      return double_;
    }
  }

 private:
  enum type_t { nil, boolean, number, object_t };

  type_t type_;
  union {
    bool boolean_;
    double double_;
    object* object_;
  };
};

}  // namespace fast

struct value {
  value() noexcept : id_{id<nil>} {}
  value(bool b) noexcept : id_{id<bool>}, boolean_{b} {}
  value(double d) noexcept : id_{id<double>}, double_{d} {}
  value(const char* str) noexcept
      : id_{id<string_ptr>}, string_{std::make_unique<std::string>(str)} {}
  value(std::string str) noexcept
      : id_{id<string_ptr>},
        string_{std::make_unique<std::string>(std::move(str))} {}

  value(const value& v) noexcept { copy(v); }
  value(value&& v) noexcept { copy(std::move(v)); }
  value& operator=(const value& v) {
    if (this != &v) {
      destory();
      copy(v);
    }
    return *this;
  }
  value& operator=(value&& v) {
    if (this != &v) {
      destory();
      copy(std::move(v));
    }
    return *this;
  }

  ~value() noexcept { destory(); }

  template <typename T>
  bool is() const noexcept {
    if constexpr (std::is_same_v<T, nil>) {
      return id_ == id<nil>;
    } else if constexpr (std::is_same_v<T, bool>) {
      return id_ == id<bool>;
    } else if constexpr (std::is_same_v<T, double>) {
      return id_ == id<double>;
    } else if constexpr (std::is_same_v<T, std::string>) {
      return id_ == id<string_ptr>;
    }
  }

  template <typename T>
  const T& as() const {
    if constexpr (std::is_same_v<T, bool>) {
      return boolean_;
    } else if constexpr (std::is_same_v<T, double>) {
      if (id_ == id<double>) {
        return double_;
      }
      throw internal_error{""};
    } else if constexpr (std::is_same_v<T, std::string>) {
      return *string_;
    }
  }

  friend value operator==(const value& lhs, const value& rhs) {
    if (lhs.id_ == rhs.id_) {
      switch (lhs.id_) {
        case id<nil>:
          return true;
        case id<bool>:
          return lhs.boolean_ == rhs.boolean_;
        case id<double>:
          return lhs.double_ == rhs.double_;
        case id<string_ptr>:
          return *lhs.string_ == *rhs.string_;
      }
    }
    return false;
  }

  std::string repr() const noexcept {
    switch (id_) {
      case id<nil>:
        return "nil";
      case id<bool>:
        return boolean_ ? "true" : "false";
      case id<double>: {
        std::ostringstream oss;
        oss << double_;
        return oss.str();
      }
      case id<string_ptr>:
        return *string_;
    }
    return "";
  }

 private:
  using string_ptr = std::unique_ptr<std::string>;
  using types = type_list<nil, bool, double, string_ptr>;
  using id_t = std::size_t;

  template <typename T>
  static constexpr id_t id = index_of<T, types>::value;

  template <typename Value>
  void copy(Value&& from) noexcept {
    id_ = from.id_;
    switch (id_) {
      case id<nil>:
        break;
      case id<bool>:
        boolean_ = from.boolean_;
        break;
      case id<double>:
        double_ = from.double_;
        break;
      case id<string_ptr>:
        copy_string(std::forward<Value>(from));
        break;
    }
  }

  void copy_string(const value& from) {
    new (&string_) string_ptr{new std::string{*from.string_}};
  }

  void copy_string(value&& from) {
    from.id_ = id<nil>;
    new (&string_) string_ptr{std::move(from.string_)};
  }

  void destory() noexcept {
    if (id_ == id<string_ptr>) {
      string_.~unique_ptr();
    }
  }

  id_t id_;
  union {
    bool boolean_;
    double double_;
    string_ptr string_;
  };
};

inline value operator+(const value& lhs, const value& rhs) {
  if (lhs.is<double>() && rhs.is<double>()) {
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

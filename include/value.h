#ifndef LOX_VALUE_H
#define LOX_VALUE_H

#include <string>

#include "exception.h"
#include "object.h"
#include "type_list.h"

namespace lox {

struct nil {};

namespace optimized {
struct value {
  value() noexcept : bits_{make_nil()} {}
  value(bool b) noexcept : bits_{make_bool(b)} {}
  value(double d) noexcept : double_{d} {}
  value(object* obj) noexcept : bits_{make_object(obj)} {}

  template <typename T>
  bool is() const {
    if constexpr (std::is_same_v<T, nil>) {
      return is_nil();
    } else if constexpr (std::is_same_v<T, bool>) {
      return is_bool();
    } else if constexpr (std::is_same_v<T, double>) {
      return is_number();
    } else if constexpr (std::is_same_v<T, object*>) {
      return is_object();
    }
    throw internal_error{"Unknown value type."};
  }

  template <typename T>
  const T as() const {
    if constexpr (std::is_same_v<T, bool>) {
      ENSURES(is_bool());
      return (bits_ & (tag_true | qnan)) == (tag_true | qnan);
    } else if constexpr (std::is_same_v<T, double>) {
      ENSURES(is_number());
      return double_;
    } else if constexpr (std::is_same_v<T, object*>) {
      ENSURES(is_object());
      return reinterpret_cast<object*>(bits_ & ~(tag_object | qnan));
    }
    throw internal_error{"Unknown value type."};
  }

  friend bool operator==(const value& lhs, const value& rhs) noexcept {
    if (lhs.is_nil() && rhs.is_nil()) {
      return true;
    }
    if (lhs.is_bool() && rhs.is_bool()) {
      return lhs.as<bool>() == rhs.as<bool>();
    }
    if (lhs.is_number() && rhs.is_number()) {
      return lhs.as<double>() == rhs.as<double>();
    }
    if (lhs.is_object() && rhs.is_object()) {
      return lhs.as<object*>() == rhs.as<object*>();
    }
    return false;
  }

  std::string repr() const {
    if (is_nil()) {
      return "nil";
    }
    if (is_bool()) {
      return as<bool>() ? "true" : "false";
    }
    if (is_number()) {
      return std::to_string(as<double>());
    }
    if (is_object()) {
      return "object";
    }
    throw internal_error{""};
  }

 private:
  using value_t = uint64_t;

  constexpr static value_t qnan = 0x7ffc000000000000;
  constexpr static value_t tag_nil = 1;
  constexpr static value_t tag_false = 2;
  constexpr static value_t tag_true = 3;
  constexpr static value_t tag_object = 0x8000000000000000;

  constexpr static value_t make_nil() noexcept { return tag_nil | qnan; }
  constexpr static value_t make_bool(bool b) noexcept {
    return (b ? tag_true : tag_false) | qnan;
  }
  static value_t make_object(object* obj) noexcept {
    return tag_object | qnan | reinterpret_cast<uint64_t>(obj);
  }

  constexpr bool is_nil() const noexcept { return bits_ == make_nil(); }
  constexpr bool is_bool() const noexcept {
    return (bits_ & (tag_false | qnan)) == (tag_false | qnan);
  }
  constexpr bool is_number() const noexcept { return (bits_ & qnan) != qnan; }
  constexpr bool is_object() const noexcept {
    return (bits_ & (tag_object | qnan)) != (tag_object | qnan);
  }

  union {
    double double_;
    value_t bits_;
  };
};

}  // namespace optimized

namespace tagged_union {

struct value {
  value() noexcept : type_{nil_t} {}
  value(bool b) noexcept : type_{boolean}, boolean_{b} {}
  value(double d) noexcept : type_{number}, double_{d} {}
  value(object* o) noexcept : type_{object_t}, object_{o} {}

  template <typename T>
  bool is() const {
    if constexpr (std::is_same_v<T, nil>) {
      return type_ == nil_t;
    } else if constexpr (std::is_same_v<T, bool>) {
      return type_ == boolean;
    } else if constexpr (std::is_same_v<T, double>) {
      return type_ == number;
    } else if constexpr (std::is_same_v<T, object*>) {
      return type_ == object_t;
    }
    throw internal_error{"Unknown value type."};
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
    throw internal_error{"Unknown value type."};
  }

  friend bool operator==(const value& lhs, const value& rhs) noexcept {
    if (lhs.type_ == rhs.type_) {
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
    return false;
  }

  std::string repr() const {
    switch (type_) {
      case nil_t:
        return "nil";
      case boolean:
        return boolean_ ? "true" : "false";
      case number:
        return std::to_string(double_);
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

}  // namespace tagged_union

using value = optimized::value;

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

#ifndef LOX_VALUE_H
#define LOX_VALUE_H

#include "exception.h"
#include "types.h"

namespace lox {

struct value {
  using types = type_list<null, bool, int, double>;

  template <typename T>
  static constexpr type_id id = type_wrapper<T, types>::id;

  constexpr value() noexcept : type{id<null>}, null_value{} {}
  constexpr value(bool b) noexcept : type{id<bool>}, bool_value{b} {}
  constexpr value(int i) noexcept : type{id<int>}, int_value{i} {}
  constexpr value(double d) noexcept : type{id<double>}, double_value{d} {}

  value(const value& v) : type{v.type} { copy(v); }
  value(value&& v) : type{v.type} { move(std::move(v)); }

  value& operator=(const value& v) noexcept {
    type = v.type;
    copy(v);
    return *this;
  }

  value& operator=(value&& v) noexcept {
    type = v.type;
    move(std::move(v));
    return *this;
  }

  constexpr bool empty() const noexcept { return type == id<null>; }

  template <typename T>
  constexpr bool is_type() const noexcept {
    return type == id<T>;
  }

  template <typename T>
  const T& as() const {
    if (type == id<T>) {
      if constexpr (std::is_same_v<T, null>) {
        return null_value;
      }
      if constexpr (std::is_same_v<T, bool>) {
        return bool_value;
      }
      if constexpr (std::is_same_v<T, int>) {
        return int_value;
      }
      if constexpr (std::is_same_v<T, double>) {
        return double_value;
      }
    }
    throw runtime_error{"Unknown value type (as): " + std::to_string(type)};
  }

  template <typename Visitor>
  auto visit(Visitor visitor) const {
    switch (type) {
      case id<null>:
        return visitor(null_value);
      case id<bool>:
        return visitor(bool_value);
      case id<int>:
        return visitor(int_value);
      case id<double>:
        return visitor(double_value);
      default:
        throw runtime_error{"Unexpected value type."};
    }
  }

  friend bool operator==(const value& lhs, const value& rhs) {
    if (lhs.type == value::id<int> && rhs.type == value::id<double>) {
      return lhs.int_value == rhs.double_value;
    }
    if (lhs.type == value::id<double> && rhs.type == value::id<int>) {
      return lhs.double_value == rhs.int_value;
    }
    if (lhs.type != rhs.type) return false;
    switch (lhs.type) {
      case value::id<null>:
        return true;
      case value::id<bool>:
        return lhs.bool_value == rhs.bool_value;
      case value::id<int>:
        return lhs.int_value == rhs.int_value;
      case value::id<double>:
        return lhs.double_value == rhs.double_value;
    }
    throw runtime_error{"Unknown value type (operator==)."};
  }

 private:
  void copy(const value& v) {
    switch (type) {
      case id<null>:
        null_value = null{};
        break;
      case id<bool>:
        bool_value = v.bool_value;
        break;
      case id<int>:
        int_value = v.int_value;
        break;
      case id<double>:
        double_value = v.double_value;
        break;
      default:
        throw runtime_error{"Unknow value type (copy)."};
    }
  }

  void move(value&& v) {
    switch (type) {
      case id<null>:
        null_value = null{};
        break;
      case id<bool>:
        bool_value = v.bool_value;
        break;
      case id<int>:
        int_value = v.int_value;
        break;
      case id<double>:
        double_value = v.double_value;
        break;
      default:
        throw runtime_error{"Unknow value type (move)."};
    }
  }

  type_id type;
  union {
    null null_value;
    bool bool_value;
    int int_value;
    double double_value;
  };
};

inline bool operator!=(const value& lhs, const value& rhs) noexcept {
  return !(lhs == rhs);
}

template <typename Oper>
inline value arithmetic(const value& lhs, const value& rhs, Oper op) {
  if (lhs.is_type<int>()) {
    if (rhs.is_type<int>()) {
      return value{op(lhs.as<int>(), rhs.as<int>())};
    }
    if (rhs.is_type<double>()) {
      return value{op(lhs.as<int>(), rhs.as<double>())};
    }
  }
  if (lhs.is_type<double>()) {
    if (rhs.is_type<int>()) {
      return value{op(lhs.as<double>(), rhs.as<int>())};
    }
    if (rhs.is_type<double>()) {
      return value{op(lhs.as<double>(), rhs.as<double>())};
    }
  }
  throw runtime_error{"Expect arithmetic value."};
}

inline value operator+(const value& lhs, const value& rhs) {
  return arithmetic(lhs, rhs,
                    [](const auto& v1, const auto& v2) { return v1 + v2; });
}
inline value operator-(const value& lhs, const value& rhs) {
  return arithmetic(lhs, rhs, [](auto&& v1, auto&& v2) { return v1 - v2; });
}

inline value operator*(const value& lhs, const value& rhs) {
  return arithmetic(lhs, rhs, [](auto&& v1, auto&& v2) { return v1 * v2; });
}

inline value operator/(const value& lhs, const value& rhs) {
  return arithmetic(lhs, rhs, [](auto&& v1, auto&& v2) { return v1 / v2; });
}

inline value operator>(const value& lhs, const value& rhs) {
  return arithmetic(lhs, rhs, [](auto&& v1, auto&& v2) { return v1 > v2; });
}

inline value operator<(const value& lhs, const value& rhs) {
  return arithmetic(lhs, rhs, [](auto&& v1, auto&& v2) { return v1 < v2; });
}

inline value operator>=(const value& lhs, const value& rhs) {
  return arithmetic(lhs, rhs, [](auto&& v1, auto&& v2) { return v1 >= v2; });
}

inline value operator<=(const value& lhs, const value& rhs) {
  return arithmetic(lhs, rhs, [](auto&& v1, auto&& v2) { return v1 <= v2; });
}

template <typename Oper>
inline value logic(const value& lhs, const value& rhs, Oper op) {
  if (lhs.is_type<bool>() && rhs.is_type<bool>()) {
    return value{op(lhs.as<bool>(), rhs.as<bool>())};
  }
  throw runtime_error{"Expect logic value."};
}

inline value operator||(const value& lhs, const value& rhs) {
  return logic(lhs, rhs, [](auto&& v1, auto&& v2) { return v1 || v2; });
}

inline value operator&&(const value& lhs, const value& rhs) {
  return logic(lhs, rhs, [](auto&& v1, auto&& v2) { return v1 && v2; });
}

inline string to_string(const value& v) noexcept {
  return v.visit(overloaded{
      [](null) { return string{"null"}; },
      [](bool b) { return b ? string{"true"} : string{"false"}; },
      [](int i) { return std::to_string(i); },
      [](double d) { return std::to_string(d); },
  });
}

}  // namespace lox

#endif

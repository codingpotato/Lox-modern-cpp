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

  constexpr bool empty() const noexcept { return type == id<null>; }

  template <typename T>
  constexpr bool is_type() const noexcept {
    return type == id<T>;
  }

  template <typename T>
  const T& as() const {
    throw runtime_error{"Unknown value type."};
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
    throw runtime_error{"Unknown value type."};
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
        throw runtime_error{"Unknow value type."};
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
        throw runtime_error{"Unknow value type."};
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
  return arithmetic(lhs, rhs, [](auto&& v1, auto&& v2) { return v1 + v2; });
}

}  // namespace lox

#endif

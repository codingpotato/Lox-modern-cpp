#ifndef LOX_VALUE_H
#define LOX_VALUE_H

#include <string>

#include "contract.h"
#include "exception.h"
#include "type_list.h"

namespace lox {

struct object;

namespace optimized {

struct value {
  constexpr value() noexcept : bits_{make_nil()} {}
  constexpr value(bool b) noexcept : bits_{make_bool(b)} {}
  constexpr value(double d) noexcept : double_{d} {}
  value(const object* obj) noexcept : bits_{make_object(obj)} {}

  constexpr bool is_nil() const noexcept { return bits_ == make_nil(); }
  constexpr bool is_bool() const noexcept {
    return (bits_ & (tag_false | qnan)) == (tag_false | qnan);
  }
  constexpr bool is_double() const noexcept { return (bits_ & qnan) != qnan; }
  constexpr bool is_object() const noexcept {
    return (bits_ & (tag_object | qnan)) == (tag_object | qnan);
  }

  constexpr bool as_bool() const noexcept {
    ENSURES(is_bool());
    return (bits_ & (tag_true | qnan)) == (tag_true | qnan);
  }
  constexpr double as_double() const noexcept {
    ENSURES(is_double());
    return double_;
  }
  const object* as_object() const noexcept {
    ENSURES(is_object());
    return reinterpret_cast<object*>(bits_ & ~(tag_object | qnan));
  }

 private:
  using storage_t = uint64_t;

  constexpr static storage_t qnan = 0x7ffc000000000000;
  constexpr static storage_t tag_nil = 1;
  constexpr static storage_t tag_false = 2;
  constexpr static storage_t tag_true = 3;
  constexpr static storage_t tag_object = 0x8000000000000000;

  constexpr static storage_t make_nil() noexcept { return tag_nil | qnan; }
  constexpr static storage_t make_bool(bool b) noexcept {
    return (b ? tag_true : tag_false) | qnan;
  }
  static storage_t make_object(const object* obj) noexcept {
    return tag_object | qnan | reinterpret_cast<storage_t>(obj);
  }

  union {
    double double_;
    storage_t bits_;
  };
};

}  // namespace optimized

namespace tagged_union {

struct value {
  constexpr value() noexcept : id_{id<nil>}, object_{nullptr} {}
  constexpr value(bool b) noexcept : id_{id<bool>}, bool_{b} {}
  constexpr value(double d) noexcept : id_{id<double>}, double_{d} {}
  constexpr value(object* obj) noexcept : id_{id<object>}, object_{obj} {}

  constexpr bool is_nil() const noexcept { return id_ == id<nil>; }
  constexpr bool is_bool() const noexcept { return id_ == id<bool>; }
  constexpr bool is_double() const noexcept { return id_ == id<double>; }
  constexpr bool is_object() const noexcept { return id_ == id<object>; }

  constexpr bool as_bool() const noexcept {
    ENSURES(is_bool());
    return bool_;
  }
  constexpr double as_double() const noexcept {
    ENSURES(is_double());
    return double_;
  }
  const object* as_object() const noexcept {
    ENSURES(is_object());
    return object_;
  }

 private:
  struct nil {};
  using types = type_list<nil, bool, double, object>;
  template <typename T>
  constexpr static size_t id = index_of<T, types>::value;

  size_t id_;
  union {
    bool bool_;
    double double_;
    object* object_;
  };
};

}  // namespace tagged_union

using value = optimized::value;

inline constexpr bool operator==(const value& lhs, const value& rhs) noexcept {
  if (lhs.is_nil() && rhs.is_nil()) {
    return true;
  }
  if (lhs.is_bool() && rhs.is_bool()) {
    return lhs.as_bool() == rhs.as_bool();
  }
  if (lhs.is_double() && rhs.is_double()) {
    return lhs.as_double() == rhs.as_double();
  }
  if (lhs.is_object() && rhs.is_object()) {
    return lhs.as_object() == rhs.as_object();
  }
  return false;
}

inline constexpr value operator+(const value& lhs, const value& rhs) noexcept {
  return lhs.as_double() + rhs.as_double();
}

inline constexpr value operator-(const value& lhs, const value& rhs) noexcept {
  return lhs.as_double() - rhs.as_double();
}

inline constexpr value operator*(const value& lhs, const value& rhs) noexcept {
  return lhs.as_double() * rhs.as_double();
}

inline constexpr value operator/(const value& lhs, const value& rhs) noexcept {
  return lhs.as_double() / rhs.as_double();
}

inline constexpr value operator>(const value& lhs, const value& rhs) noexcept {
  return lhs.as_double() > rhs.as_double();
}

inline constexpr value operator<(const value& lhs, const value& rhs) noexcept {
  return lhs.as_double() < rhs.as_double();
}

std::string to_string(value v, bool verbose = false) noexcept;

}  // namespace lox

#endif

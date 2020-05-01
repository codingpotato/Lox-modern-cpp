#ifndef LOX_VALUE_H
#define LOX_VALUE_H

#include <string>
#include <utility>
#include <vector>

#include "contract.h"
#include "exception.h"
#include "type_list.h"

namespace lox {

class Object;

namespace optimized {

struct Value {
  constexpr Value() noexcept : bits{make_nil()} {}
  constexpr Value(bool b) noexcept : bits{make_bool(b)} {}
  constexpr Value(double d) noexcept : double_val{d} {}
  Value(Object* obj) noexcept : bits{make_object(obj)} {}

  constexpr bool is_nil() const noexcept { return bits == make_nil(); }
  constexpr bool is_bool() const noexcept {
    return (bits & (tag_false | qnan)) == (tag_false | qnan);
  }
  constexpr bool is_double() const noexcept { return (bits & qnan) != qnan; }
  constexpr bool is_object() const noexcept {
    return (bits & (tag_object | qnan)) == (tag_object | qnan);
  }

  constexpr bool as_bool() const noexcept {
    ENSURES(is_bool());
    return (bits & (tag_true | qnan)) == (tag_true | qnan);
  }
  constexpr double as_double() const noexcept {
    ENSURES(is_double());
    return double_val;
  }
  const Object* as_object() const noexcept {
    ENSURES(is_object());
    return reinterpret_cast<Object*>(bits & ~(tag_object | qnan));
  }
  Object* as_object() noexcept {
    return const_cast<Object*>(std::as_const(*this).as_object());
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
  static storage_t make_object(const Object* obj) noexcept {
    return tag_object | qnan | reinterpret_cast<storage_t>(obj);
  }

  union {
    double double_val;
    storage_t bits;
  };
};

}  // namespace optimized

namespace tagged_union {

struct Value {
  constexpr Value() noexcept : id{id_of<Nil>}, object_val{nullptr} {}
  constexpr Value(bool val) noexcept : id{id_of<bool>}, bool_val{val} {}
  constexpr Value(double val) noexcept : id{id_of<double>}, double_val{val} {}
  constexpr Value(Object* obj) noexcept : id{id_of<Object>}, object_val{obj} {}

  constexpr bool is_nil() const noexcept { return id == id_of<Nil>; }
  constexpr bool is_bool() const noexcept { return id == id_of<bool>; }
  constexpr bool is_double() const noexcept { return id == id_of<double>; }
  constexpr bool is_object() const noexcept { return id == id_of<Object>; }

  constexpr bool as_bool() const noexcept {
    ENSURES(is_bool());
    return bool_val;
  }
  constexpr double as_double() const noexcept {
    ENSURES(is_double());
    return double_val;
  }
  const Object* as_object() const noexcept {
    ENSURES(is_object());
    return object_val;
  }
  Object* as_object() noexcept {
    return const_cast<Object*>(std::as_const(*this).as_object());
  }

 private:
  struct Nil {};
  using Types = Type_list<Nil, bool, double, Object>;
  template <typename T>
  constexpr static size_t id_of = Index_of<T, Types>::value;

  size_t id;
  union {
    bool bool_val;
    double double_val;
    Object* object_val;
  };
};

}  // namespace tagged_union

using Value = optimized::Value;
using Value_vector = std::vector<Value>;

inline constexpr bool operator==(Value lhs, Value rhs) noexcept {
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

inline constexpr Value operator+(const Value& lhs, const Value& rhs) noexcept {
  return lhs.as_double() + rhs.as_double();
}

inline constexpr Value operator-(const Value& lhs, const Value& rhs) noexcept {
  return lhs.as_double() - rhs.as_double();
}

inline constexpr Value operator*(const Value& lhs, const Value& rhs) noexcept {
  return lhs.as_double() * rhs.as_double();
}

inline constexpr Value operator/(const Value& lhs, const Value& rhs) noexcept {
  return lhs.as_double() / rhs.as_double();
}

inline constexpr Value operator>(const Value& lhs, const Value& rhs) noexcept {
  return lhs.as_double() > rhs.as_double();
}

inline constexpr Value operator<(const Value& lhs, const Value& rhs) noexcept {
  return lhs.as_double() < rhs.as_double();
}

inline bool is_falsey(Value value) noexcept {
  return value.is_nil() || (value.is_bool() && !value.as_bool());
}

std::string to_string(Value v, bool verbose = false) noexcept;

}  // namespace lox

#endif

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
  constexpr explicit value(bool b) noexcept : type{id<bool>}, bool_value{b} {}
  constexpr explicit value(int i) noexcept : type{id<int>}, int_value{i} {}
  constexpr explicit value(double d) noexcept
      : type{id<double>}, double_value{d} {}

  value(const value& v) : type{v.type} { copy(v); }
  value(value&& v) : type{v.type} { move(std::move(v)); }

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
        throw runtime_error("Unknow value type.");
    }
  }

  void move(value&& v) noexcept {
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
        throw runtime_error("Unknow value type.");
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

}  // namespace lox

#endif

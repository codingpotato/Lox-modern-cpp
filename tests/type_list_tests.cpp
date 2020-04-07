#include <doctest/doctest.h>

#include "type_list.h"

TEST_CASE("index of types") {
  static_assert(lox::Index_of<int, int, float, double>::value == 0);
  static_assert(lox::Index_of<float, int, float, double>::value == 1);
  static_assert(lox::Index_of<double, int, float, double>::value == 2);
}

TEST_CASE("index of type list") {
  using types = lox::Type_list<int, float, double>;
  static_assert(lox::Index_of<int, types>::value == 0);
  static_assert(lox::Index_of<float, types>::value == 1);
  static_assert(lox::Index_of<double, types>::value == 2);
}

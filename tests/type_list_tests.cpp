#include <doctest.h>

#include "type_list.h"

TEST_CASE("type list") {
  static_assert(lox::index_of<int, int, float, double>::value == 0);
  static_assert(lox::index_of<float, int, float, double>::value == 1);
  static_assert(lox::index_of<double, int, float, double>::value == 2);
}

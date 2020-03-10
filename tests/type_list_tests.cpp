#include <doctest.h>

#include "type_list.h"

TEST_CASE("type list") {
  using types = lox::type_list<int, float, double>;
  static_assert(lox::index_of<int, types>::value == 2);
}

#include <doctest.h>

#include "expression.h"

TEST_CASE("") {
  lox::expression{std::in_place_type<lox::expression::assignment>, 1, 2};
}

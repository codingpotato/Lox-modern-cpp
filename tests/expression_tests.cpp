#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "expression.h"

TEST_CASE("element", "[epression]") {
  lox::expression::element e{std::in_place_type<lox::expression::binary>, 1,
                             lox::expression::plus, 2};
  REQUIRE(std::get<lox::expression::binary>(e).left == 1);
}
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "expression.h"

TEST_CASE("element", "[epression]") {
  lox::expression::element element{std::in_place_type<lox::expression::binary>,
                                   1, lox::operator_t::plus, 2};
  const auto& binary = std::get<lox::expression::binary>(element);
  REQUIRE(binary.left == 1);
  REQUIRE(binary.oper == lox::operator_t::plus);
  REQUIRE(binary.right == 2);
}

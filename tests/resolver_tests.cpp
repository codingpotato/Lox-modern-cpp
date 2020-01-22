#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "resolver.h"

TEST_CASE("resolve variables", "[resolver]") {
  lox::resolver_t resolver;
  resolver.begin_scope();
  resolver.declear("a");
  resolver.define("a");
  resolver.declear("b");
  resolver.define("b");
  resolver.begin_scope();
  resolver.declear("c");
  resolver.define("c");
  resolver.declear("d");
  resolver.define("d");
  const auto info_a = resolver.resolve("a");
  REQUIRE(info_a.distance == 1);
  REQUIRE(info_a.index == 0);
  const auto info_b = resolver.resolve("b");
  REQUIRE(info_b.distance == 1);
  REQUIRE(info_b.index == 1);
  const auto info_c = resolver.resolve("c");
  REQUIRE(info_c.distance == 0);
  REQUIRE(info_c.index == 0);
  const auto info_d = resolver.resolve("d");
  REQUIRE(info_d.distance == 0);
  REQUIRE(info_d.index == 1);
}

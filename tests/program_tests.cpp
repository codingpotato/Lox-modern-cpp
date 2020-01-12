#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "program.h"

TEST_CASE("string cache", "[program]") {
  const std::string string1 = "string1";
  const std::string string2 = "string2";
  lox::string_cache cache;
  auto id1 = cache.add(string1);
  REQUIRE(cache.size() == 1);
  REQUIRE(cache.get(id1) == string1);
  cache.add(string1);
  REQUIRE(cache.size() == 1);
  auto id2 = cache.add(string2);
  REQUIRE(cache.size() == 2);
  REQUIRE(cache.get(id1) == string1);
  REQUIRE(cache.get(id2) == string2);
}

TEST_CASE("add string", "[program]") {
  lox::program program;
  REQUIRE(program.strings.add("string") == 0);
}

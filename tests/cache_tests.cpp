#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "cache.h"

TEST_CASE("size of expression", "[cache]") {
  lox::cache<double> cache;
  cache.add(1.0);
  CHECK(cache.size() == 1);
  CHECK(cache.get(0) == 1.0);
}

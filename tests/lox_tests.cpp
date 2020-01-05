#define CATCH_CONFIG_MAIN
#include <algorithm>
#include <catch2/catch.hpp>

TEST_CASE("simple", "[tests]") { REQUIRE(1 == 1); }
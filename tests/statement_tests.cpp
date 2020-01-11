#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "statement.h"

TEST_CASE("scan", "[statements]") { REQUIRE(sizeof(lox::statement) == 4); }

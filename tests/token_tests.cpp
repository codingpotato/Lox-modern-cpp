#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "token.h"

TEST_CASE("scan", "[token]") {
  CHECK(sizeof(lox::token::literal) == 4);
  CHECK(sizeof(std::string) == 4);
}
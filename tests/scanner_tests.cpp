#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "scanner.h"

using namespace std;
using namespace lox;

TEST_CASE("scan", "[scanner]") {
  REQUIRE(scan(")") == vector<token>{token{token::right_paren}});
}
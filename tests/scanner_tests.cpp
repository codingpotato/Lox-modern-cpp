#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "scanner.h"

TEST_CASE("scan", "[scanner]") {
  lox::scanner scanner("test");
  auto s = std::move(scanner);
}
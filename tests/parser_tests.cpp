#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "parser.h"

TEST_CASE("parse", "[parser]") { lox::parser parser{}; }
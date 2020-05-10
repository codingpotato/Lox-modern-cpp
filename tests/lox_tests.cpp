#include <doctest/doctest.h>

#include <string>

#include "config.h"
#include "helper.h"

#define LOX_TEST_CASE(filename)                                       \
  TEST_CASE("lox: " filename) {                                       \
    auto [source, expected] = load(EXAMPLES_DIR "/" filename ".lox"); \
    REQUIRE_EQ(run(source), expected);                                \
  }

LOX_TEST_CASE("assignment/associativity")
LOX_TEST_CASE("assignment/global")
LOX_TEST_CASE("assignment/grouping")
LOX_TEST_CASE("assignment/infix_operator")
LOX_TEST_CASE("assignment/local")
LOX_TEST_CASE("assignment/prefix_operator")
LOX_TEST_CASE("assignment/syntax")
LOX_TEST_CASE("assignment/undefined")

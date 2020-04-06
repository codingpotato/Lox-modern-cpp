#include <doctest/doctest.h>

#include <iostream>
#include <vector>

#include "performance.h"
#include "value.h"

TEST_CASE("nil value") {
  const lox::value v{};
  CHECK(v.is_nil());
}

TEST_CASE("bool value") {
  lox::value v{true};
  CHECK(v.is_bool());
  CHECK(v.as_bool());
  v = false;
  CHECK(v.is_bool());
  CHECK(!v.as_bool());
}

TEST_CASE("double value") {
  lox::value v{1.0};
  CHECK(v.is_double());
  CHECK_EQ(v.as_double(), 1);
  v = 2.0;
  CHECK(v.is_double());
  CHECK_EQ(v.as_double(), 2);
}

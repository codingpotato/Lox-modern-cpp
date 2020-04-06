#include <doctest/doctest.h>

#include <iostream>
#include <vector>

#include "performance.h"
#include "value.h"

TEST_CASE("nil value") {
  const lox::Value value{};
  CHECK(value.is_nil());
}

TEST_CASE("bool value") {
  lox::Value value{true};
  CHECK(value.is_bool());
  CHECK(value.as_bool());
  value = false;
  CHECK(value.is_bool());
  CHECK(!value.as_bool());
}

TEST_CASE("double value") {
  lox::Value value{1.0};
  CHECK(value.is_double());
  CHECK_EQ(value.as_double(), 1);
  value = 2.0;
  CHECK(value.is_double());
  CHECK_EQ(value.as_double(), 2);
}

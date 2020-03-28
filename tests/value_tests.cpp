#include <doctest/doctest.h>

#include <iostream>
#include <vector>

#include "performance.h"
#include "value.h"

TEST_CASE("nil value") {
  const lox::value v{};
  CHECK(v.is<lox::nil>());
}

TEST_CASE("bool value") {
  lox::value v{true};
  CHECK(v.is<bool>());
  CHECK(v.as<bool>());
  v = false;
  CHECK(v.is<bool>());
  CHECK(!v.as<bool>());
}

TEST_CASE("double value") {
  lox::value v{1.0};
  CHECK(v.is<double>());
  CHECK_EQ(v.as<double>(), 1);
  v = 2.0;
  CHECK(v.is<double>());
  CHECK_EQ(v.as<double>(), 2);
}

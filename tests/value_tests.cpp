#include <doctest.h>

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

TEST_CASE("copy string value") {
  const auto str = "test string";
  const lox::value v1{str};
  const lox::value v2{v1};
  CHECK(v1.is<std::string>());
  CHECK_EQ(v1.as<std::string>(), str);
  CHECK(v2.is<std::string>());
  CHECK_EQ(v2.as<std::string>(), str);
}

TEST_CASE("move string value") {
  const auto str = "test string";
  lox::value v1{str};
  const lox::value v2{std::move(v1)};
  CHECK(v1.is<lox::nil>());
  CHECK(v2.is<std::string>());
  CHECK_EQ(v2.as<std::string>(), str);
}

TEST_CASE("multiple type value assignment") {
  lox::value v{};
  CHECK(v.is<lox::nil>());
  v = false;
  CHECK(v.is<bool>());
  CHECK(!v.as<bool>());
  v = 1.0;
  CHECK(v.is<double>());
  CHECK_EQ(v.as<double>(), 1.0);
  const auto str = "test string";
  v = str;
  CHECK(v.is<std::string>());
  CHECK_EQ(v.as<std::string>(), str);
  v = {};
  CHECK(v.is<lox::nil>());
}

TEST_CASE("variant value performance") {
  volatile double result = 0;
  lox::measure("variant value performance", 10000000, [&]() {
    lox::variant::value value{(double)std::rand()};
    lox::variant::value v = value;
    result = v.as<double>();
  });

  lox::measure("value performance", 10000000, [&]() {
    lox::value value{(double)std::rand()};
    lox::value v = value;
    result = v.as<double>();
  });
}

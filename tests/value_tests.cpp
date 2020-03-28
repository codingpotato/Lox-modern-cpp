#include <doctest/doctest.h>

#include <iostream>
#include <vector>

#include "performance.h"
#include "value.h"
#include "vector.h"

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

TEST_CASE("value performance") {
  std::srand(std::time(nullptr));
  lox::vector<lox::value> values;
  for (std::size_t i = 0; i < 50; ++i) {
    values.emplace_back(static_cast<double>(std::rand()));
  }
  volatile double result = 0;
  lox::measure("fast value performance", 10000000, [&]() {
    result += values[std::rand() % values.size()].as<double>();
  });
  std::cout << result << "\n";

  {
    lox::vector<lox::value> values;
    for (std::size_t i = 0; i < 50; ++i) {
      values.emplace_back(static_cast<double>(std::rand()));
    }
    volatile double result = 0;
    lox::measure("value performance", 10000000, [&]() {
      result += values[std::rand() % values.size()].as<double>();
    });
    std::cout << result << "\n";
  }

  {
    lox::vector<int> values;
    for (std::size_t i = 0; i < 50; ++i) {
      values.emplace_back(static_cast<int>(std::rand()));
    }
    volatile int result = 0;
    lox::measure("uint value performance", 10000000,
                 [&]() { result += values[std::rand() % values.size()]; });
    std::cout << result << "\n";
  }
}

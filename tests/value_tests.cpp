#include <doctest/doctest.h>

#include "object.h"
#include "value.h"

TEST_CASE("value: tagged union") {
  using Value = lox::tagged_union::Value;
  SUBCASE("nil") { REQUIRE(Value{}.is_nil()); }

  SUBCASE("bool") {
    Value value{false};
    REQUIRE(value.is_bool());
    REQUIRE(!value.as_bool());
    value = true;
    REQUIRE(value.is_bool());
    REQUIRE(value.as_bool());
  }

  SUBCASE("double") {
    Value value{1.0};
    REQUIRE(value.is_double());
    REQUIRE_EQ(value.as_double(), 1);
    value = 2.0;
    REQUIRE(value.is_double());
    REQUIRE_EQ(value.as_double(), 2);
  }

  SUBCASE("object") {
    lox::String hello("hello");
    Value value{&hello};
    REQUIRE(value.is_object());
    REQUIRE_EQ(value.as_object(), &hello);
    lox::String world("world");
    value = &world;
    REQUIRE(value.is_object());
    REQUIRE_EQ(value.as_object(), &world);
  }

  SUBCASE("assignment") {
    Value value{};
    value = true;
    REQUIRE(value.is_bool());
    REQUIRE(value.as_bool());
    value = 1.0;
    REQUIRE(value.is_double());
    REQUIRE_EQ(value.as_double(), 1);
    lox::String hello("hello");
    value = &hello;
    REQUIRE(value.is_object());
    REQUIRE_EQ(value.as_object(), &hello);
  }
}

TEST_CASE("value: optimized") {
  using Value = lox::optimized::Value;
  SUBCASE("nil") { REQUIRE(Value{}.is_nil()); }

  SUBCASE("bool") {
    Value value{false};
    REQUIRE(value.is_bool());
    REQUIRE(!value.as_bool());
    value = true;
    REQUIRE(value.is_bool());
    REQUIRE(value.as_bool());
  }

  SUBCASE("double") {
    Value value{1.0};
    REQUIRE(value.is_double());
    REQUIRE_EQ(value.as_double(), 1);
    value = 2.0;
    REQUIRE(value.is_double());
    REQUIRE_EQ(value.as_double(), 2);
  }

  SUBCASE("object") {
    lox::String hello("hello");
    Value value{&hello};
    REQUIRE(value.is_object());
    REQUIRE_EQ(value.as_object(), &hello);
    lox::String world("world");
    value = &world;
    REQUIRE(value.is_object());
    REQUIRE_EQ(value.as_object(), &world);
  }

  SUBCASE("assignment") {
    Value value{};
    value = true;
    REQUIRE(value.is_bool());
    REQUIRE(value.as_bool());
    value = 1.0;
    REQUIRE(value.is_double());
    REQUIRE_EQ(value.as_double(), 1);
    lox::String hello("hello");
    value = &hello;
    REQUIRE(value.is_object());
    REQUIRE_EQ(value.as_object(), &hello);
  }
}

TEST_CASE("value: is_falsey") {
  REQUIRE(lox::is_falsey(lox::Value{}));
  REQUIRE(lox::is_falsey(lox::Value{false}));
  REQUIRE(!lox::is_falsey(lox::Value{true}));
  REQUIRE(!lox::is_falsey(lox::Value{1.0}));
}

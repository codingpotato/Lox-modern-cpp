#include <doctest/doctest.h>

#include <vector>

#include "hash_table.h"
#include "object.h"
#include "value.h"

TEST_CASE("hash table insert") {
  lox::Hash_table table;
  lox::String string{"test string"};
  table.insert(&string, lox::Value{1.0});
  CHECK_EQ(table.size(), 1);
  CHECK(table.contains(&string));
  auto value = table.get_if(&string);
  CHECK(value != nullptr);
  CHECK_EQ(value->as_double(), 1);
}

TEST_CASE("hash table insert multiple entries") {
  lox::Hash_table table;
  std::vector<lox::String> strings;
  for (auto i = 0; i < 100; ++i) {
    strings.emplace_back("test string " + std::to_string(i));
  }
  for (std::size_t i = 0; i < strings.size(); ++i) {
    table.insert(&strings[i], lox::Value{static_cast<double>(i)});
  }
  CHECK_EQ(table.size(), strings.size());
  for (std::size_t i = 0; i < strings.size(); ++i) {
    auto value = table.get_if(&strings[i]);
    CHECK(value != nullptr);
    CHECK_EQ(value->as_double(), i);
  }
}

TEST_CASE("hash table erase") {
  lox::Hash_table table;
  std::vector<lox::String> strings;
  for (auto i = 0; i < 100; ++i) {
    strings.emplace_back("test string " + std::to_string(i));
  }
  for (std::size_t i = 0; i < strings.size(); ++i) {
    table.insert(&strings[i], lox::Value{static_cast<double>(i)});
  }
  for (std::size_t i = 0; i < strings.size(); i += 2) {
    table.erase(&strings[i]);
  }
  CHECK_EQ(table.size(), strings.size() / 2);
  for (std::size_t i = 1; i < strings.size(); i += 2) {
    auto value = table.get_if(&strings[i]);
    CHECK(value != nullptr);
    CHECK_EQ(value->as_double(), i);
  }
}

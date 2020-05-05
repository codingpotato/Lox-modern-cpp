#include <doctest/doctest.h>

#include <memory>
#include <vector>

#include "hash_table.h"
#include "object.h"
#include "value.h"

TEST_CASE("hash table: insert") {
  lox::Hash_table table;
  lox::String string{"string"};
  table.insert(&string, 1.0);
  REQUIRE(table.contains(&string));
  REQUIRE(table.get_if(&string) != nullptr);
  REQUIRE_EQ(table.get_if(&string)->as_double(), 1);
  table.set(&string, 2.0);
  REQUIRE_EQ(table.get_if(&string)->as_double(), 2);
}

TEST_CASE("hash table: insert multiple entries") {
  lox::Hash_table table;
  std::vector<std::unique_ptr<lox::String>> strings;
  for (auto i = 0; i < 100; ++i) {
    strings.emplace_back(
        std::make_unique<lox::String>("string " + std::to_string(i)));
  }
  for (std::size_t i = 0; i < strings.size(); ++i) {
    table.insert(strings[i].get(), static_cast<double>(i));
  }
  for (std::size_t i = 0; i < strings.size(); ++i) {
    REQUIRE(table.get_if(strings[i].get()) != nullptr);
    REQUIRE_EQ(table.get_if(strings[i].get())->as_double(), i);
  }
}

TEST_CASE("hash table: erase") {
  lox::Hash_table table;
  std::vector<std::unique_ptr<lox::String>> strings;
  for (auto i = 0; i < 100; ++i) {
    strings.emplace_back(
        std::make_unique<lox::String>("string " + std::to_string(i)));
  }
  for (std::size_t i = 0; i < strings.size(); ++i) {
    table.insert(strings[i].get(), static_cast<double>(i));
  }
  for (std::size_t i = 0; i < strings.size(); i += 2) {
    table.erase(strings[i].get());
  }
  for (std::size_t i = 1; i < strings.size(); i += 2) {
    REQUIRE(table.get_if(strings[i].get()) != nullptr);
    REQUIRE_EQ(table.get_if(strings[i].get())->as_double(), i);
  }
}

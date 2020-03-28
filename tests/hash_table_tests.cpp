#include <doctest/doctest.h>

#include <vector>

#include "hash_table.h"

TEST_CASE("hash table insert") {
  lox::hash_table table;
  lox::string string{"test string"};
  table.insert(&string, lox::value{1.0});
  CHECK_EQ(table.size(), 1);
  CHECK(table.contains(&string));
  CHECK_EQ(table[&string].as<double>(), 1);
}

TEST_CASE("hash table insert multiple entries") {
  lox::hash_table table;
  std::vector<lox::string> strings;
  for (auto i = 0; i < 100; ++i) {
    strings.emplace_back("test string " + std::to_string(i));
  }
  for (std::size_t i = 0; i < strings.size(); ++i) {
    table.insert(&strings[i], lox::value{static_cast<double>(i)});
  }
  CHECK_EQ(table.size(), strings.size());
  for (std::size_t i = 0; i < strings.size(); ++i) {
    CHECK_EQ(table[&strings[i]].as<double>(), i);
  }
}

TEST_CASE("hash table erase") {
  lox::hash_table table;
  std::vector<lox::string> strings;
  for (auto i = 0; i < 100; ++i) {
    strings.emplace_back("test string " + std::to_string(i));
  }
  for (std::size_t i = 0; i < strings.size(); ++i) {
    table.insert(&strings[i], lox::value{static_cast<double>(i)});
  }
  for (std::size_t i = 0; i < strings.size(); i += 2) {
    table.erase(&strings[i]);
  }
  CHECK_EQ(table.size(), strings.size() / 2);
  for (std::size_t i = 1; i < strings.size(); i += 2) {
    CHECK_EQ(table[&strings[i]].as<double>(), i);
  }
}

#include <doctest/doctest.h>

#include <array>

#include "hash_table.h"

TEST_CASE("hash table") {
  constexpr int count = 11;
  std::array<lox::refectoring::string, count> keys = {
      std::string{"1"}, std::string{"2"},  std::string{"3"}, std::string{"4"},
      std::string{"5"}, std::string{"6"},  std::string{"7"}, std::string{"8"},
      std::string{"9"}, std::string{"10"}, std::string{"11"}};
  std::array<double, count> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  lox::hash_table table;
  for (std::size_t i = 0; i < keys.size(); ++i) {
    table.insert(&keys[i], lox::value{values[i]});
  }
  CHECK_EQ(table.size(), count);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    CHECK_EQ(table[&keys[i]].as<double>(), values[i]);
  }
}

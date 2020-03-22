#include <doctest/doctest.h>

#include <array>

#include "hash_table.h"

TEST_CASE("hash table") {
  struct Key {
    Key(int v) noexcept : value{v} {}
    int hash() const noexcept { return value; }
    int value;
  };

  constexpr int count = 11;
  std::array<Key, count> keys = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  std::array<double, count> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  lox::hash_table<Key, double> table;
  for (std::size_t i = 0; i < keys.size(); ++i) {
    table.insert(&keys[i], values[i]);
  }
  CHECK_EQ(table.size(), count);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    CHECK_EQ(table[&keys[i]], values[i]);
  }
}

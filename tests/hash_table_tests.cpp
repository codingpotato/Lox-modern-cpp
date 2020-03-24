#include <doctest/doctest.h>

#include <array>

#include "hash_table.h"

struct Key {
  constexpr Key(int v) noexcept : value{v} {}
  constexpr int hash() const noexcept { return value; }
  friend bool operator==(const Key& lhs, const Key& rhs) noexcept {
    return lhs.value == rhs.value;
  }

 private:
  int value;
};

TEST_CASE("hash table") {
  constexpr int count = 11;
  std::array<Key, count> keys = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  std::array<double, count> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  lox::hash_table<Key, double> table;
  for (std::size_t i = 0; i < keys.size(); ++i) {
    table.insert({&keys[i], values[i]});
  }
  CHECK_EQ(table.size(), count);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    CHECK_EQ(table[&keys[i]], values[i]);
  }
}

TEST_CASE("set") {
  constexpr int count = 11;
  std::array<Key, count> keys = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  lox::set<Key> table;
  for (std::size_t i = 0; i < keys.size(); ++i) {
    table.insert({&keys[i]});
  }
  CHECK_EQ(table.size(), count);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    CHECK(table.contains(&keys[i]));
  }
}

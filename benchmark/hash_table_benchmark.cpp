#include <benchmark/benchmark.h>

#include <array>
#include <unordered_map>

#include "hash_table.h"

struct Key {
  Key(int v) noexcept : value{v} {}
  int hash() const noexcept { return value; }
  int value;
};

static void hash_table(benchmark::State& state) {
  while (state.KeepRunning()) {
    constexpr int count = 11;
    std::array<Key, count> keys = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    std::array<double, count> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    lox::hash_table<Key, double> table;
    for (std::size_t i = 0; i < keys.size(); ++i) {
      table.insert(&keys[i], values[i]);
    }
    double result;
    for (std::size_t i = 0; i < keys.size(); ++i) {
      benchmark::DoNotOptimize(result = table[&keys[i]]);
    }
  }
}
BENCHMARK(hash_table);

static void unordered_map(benchmark::State& state) {
  while (state.KeepRunning()) {
    constexpr int count = 11;
    std::array<Key, count> keys = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    std::array<double, count> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    std::unordered_map<Key*, double> table;
    for (std::size_t i = 0; i < keys.size(); ++i) {
      table.emplace(&keys[i], values[i]);
    }
    double result;
    for (std::size_t i = 0; i < keys.size(); ++i) {
      benchmark::DoNotOptimize(result = table[&keys[i]]);
    }
  }
}
BENCHMARK(unordered_map);

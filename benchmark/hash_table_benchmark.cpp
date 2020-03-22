#include <benchmark/benchmark.h>

#include <array>
#include <unordered_map>

#include "hash_table.h"
#include "object.h"

struct Key {
  constexpr Key(int v) noexcept : value{v} {}
  constexpr int hash() const noexcept { return value; }
  friend bool operator==(const Key& lhs, const Key& rhs) noexcept {
    return lhs.value == rhs.value;
  }

 private:
  int value;
};

static void hash_table(benchmark::State& state) {
  while (state.KeepRunning()) {
    constexpr int count = 11;
    std::array<Key, count> keys = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    std::array<double, count> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    lox::hash_table<Key, double> table;
    for (std::size_t i = 0; i < keys.size(); ++i) {
      table.insert(keys[i], values[i]);
    }
    double result;
    for (std::size_t i = 0; i < keys.size(); ++i) {
      benchmark::DoNotOptimize(result = table[keys[i]]);
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

static void hash_table_string(benchmark::State& state) {
  while (state.KeepRunning()) {
    constexpr int count = 11;
    std::array<lox::object, count> keys = {
        lox::object{std::string{"1"}}, lox::object{std::string{"2"}},
        lox::object{std::string{"3"}}, lox::object{std::string{"4"}},
        lox::object{std::string{"5"}}, lox::object{std::string{"6"}},
        lox::object{std::string{"7"}}, lox::object{std::string{"8"}},
        lox::object{std::string{"9"}}, lox::object{std::string{"10"}},
        lox::object{std::string{"11"}}};
    std::array<double, count> values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    lox::hash_table<lox::object, double> table;
    for (std::size_t i = 0; i < keys.size(); ++i) {
      table.insert(keys[i], values[i]);
    }
    double result;
    for (std::size_t i = 0; i < keys.size(); ++i) {
      benchmark::DoNotOptimize(result = table[keys[i]]);
    }
  }
}
BENCHMARK(hash_table_string);

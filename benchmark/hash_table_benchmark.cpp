#include <benchmark/benchmark.h>

#include <unordered_map>

#include "hash_table.h"
#include "object.h"

static void hash_table(benchmark::State& state) {
  while (state.KeepRunning()) {
    lox::hash_table table;
    std::vector<lox::string> strings;
    for (auto i = 0; i < 100; ++i) {
      strings.emplace_back("test string " + std::to_string(i));
    }
    for (std::size_t i = 0; i < strings.size(); ++i) {
      table.insert(&strings[i], lox::value{static_cast<double>(i)});
    }
    double result = 0;
    for (std::size_t i = 0; i < strings.size(); ++i) {
      benchmark::DoNotOptimize(result += table[&strings[i]].as<double>());
    }
  }
}
BENCHMARK(hash_table);

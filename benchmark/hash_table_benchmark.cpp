#include <benchmark/benchmark.h>

#include <unordered_map>

#include "hash_table.h"
#include "object.h"

static void hash_table(benchmark::State& state) {
  while (state.KeepRunning()) {
    lox::Hash_table table;
    std::vector<lox::String> strings;
    for (auto i = 0; i < 100; ++i) {
      strings.emplace_back("test string " + std::to_string(i));
    }
    for (std::size_t i = 0; i < strings.size(); ++i) {
      table.insert(&strings[i], lox::Value{static_cast<double>(i)});
    }
    double result = 0;
    for (std::size_t i = 0; i < strings.size(); ++i) {
      if (auto value = table.get_if(&strings[i]); value != nullptr) {
        benchmark::DoNotOptimize(result += value->as_double());
      }
    }
  }
}
BENCHMARK(hash_table);

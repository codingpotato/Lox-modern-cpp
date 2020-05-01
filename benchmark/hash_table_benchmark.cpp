#include <benchmark/benchmark.h>

#include <memory>

#include "hash_table.h"
#include "object.h"

static void hash_table(benchmark::State& state) {
  while (state.KeepRunning()) {
    lox::Hash_table table;
    std::vector<std::unique_ptr<lox::String>> strings;
    for (auto i = 0; i < 100; ++i) {
      strings.emplace_back(
          std::make_unique<lox::String>("test string " + std::to_string(i)));
    }
    for (std::size_t i = 0; i < strings.size(); ++i) {
      table.set(strings[i].get(), lox::Value{static_cast<double>(i)});
    }
    double result = 0;
    for (std::size_t i = 0; i < strings.size(); ++i) {
      if (auto value = table.get_if(strings[i].get()); value != nullptr) {
        benchmark::DoNotOptimize(result += value->as_double());
      }
    }
  }
}
BENCHMARK(hash_table);

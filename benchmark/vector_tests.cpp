#include <benchmark/benchmark.h>

#include "value.h"
#include "vector.h"

constexpr int count = 1000000;

static void vector(benchmark::State& state) {
  lox::vector<lox::value> vector;
  for (int i = 0; i < count; ++i) {
    vector.emplace_back(static_cast<double>(i));
  }
  while (state.KeepRunning()) {
    double s = 0;
    for (int i = 0; i < vector.size(); ++i) {
      benchmark::DoNotOptimize(s += vector[i].as<double>());
    }
  }
}
BENCHMARK(vector);

static void std_vector(benchmark::State& state) {
  std::vector<lox::value> vector;
  for (int i = 0; i < count; ++i) {
    vector.emplace_back(static_cast<double>(i));
  }
  while (state.KeepRunning()) {
    double s = 0;
    for (int i = 0; i < vector.size(); ++i) {
      benchmark::DoNotOptimize(s += vector[i].as<double>());
    }
  }
}
BENCHMARK(std_vector);

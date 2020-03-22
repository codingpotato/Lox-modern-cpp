#include <benchmark/benchmark.h>

#include "value.h"
#include "vector.h"

constexpr int count = 100;

static void vector(benchmark::State& state) {
  lox::vector<lox::value> vector;
  vector.emplace_back(static_cast<double>(100.0));
  double s = 0;
  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(s = vector[0].as<double>());
  }
}
BENCHMARK(vector);

static void std_vector(benchmark::State& state) {
  std::vector<lox::value> vector;
  vector.emplace_back(static_cast<double>(100.0));
  double s = 0;
  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(s = vector[0].as<double>());
  }
}
BENCHMARK(std_vector);

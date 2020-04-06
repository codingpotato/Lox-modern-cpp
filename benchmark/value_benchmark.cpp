#include <benchmark/benchmark.h>

#include "value.h"

static void value(benchmark::State& state) {
  lox::Value value{1.0};
  lox::Value result;
  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(result = value);
  }
}
BENCHMARK(value);

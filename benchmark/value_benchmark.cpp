#include <benchmark/benchmark.h>

#include "value.h"
#include "vector.h"

static void value(benchmark::State& state) {
  lox::value value{1.0};
  lox::value result;
  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(result = value);
  }
}
BENCHMARK(value);

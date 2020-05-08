#include <benchmark/benchmark.h>

#include "config.h"
#include "helper.h"

BENCHMARK_MAIN();

static void fib(benchmark::State& state) {
  auto source = load_source(lox_benchmark_fib);
  while (state.KeepRunning()) {
    run(source);
  }
}
BENCHMARK(fib);

static void sum(benchmark::State& state) {
  auto source = load_source(lox_benchmark_sum);
  while (state.KeepRunning()) {
    run(source);
  }
}
BENCHMARK(sum);

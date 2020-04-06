#include <benchmark/benchmark.h>

#include "helper.h"

static void fib(benchmark::State& state) {
  const std::string source{R"(
fun fib(n) {
  if (n < 2) return n;
  return fib(n - 2) + fib(n - 1);
}
print fib(30);
)"};
  while (state.KeepRunning()) {
    run(source);
  }
}
BENCHMARK(fib);

#include <benchmark/benchmark.h>

#include <fstream>

#include "config.h"
#include "helper.h"

static void sum(benchmark::State& state) {
  std::ifstream ifs(benchmark_sum_lox);
  const auto source = std::string{std::istreambuf_iterator<char>{ifs},
                  std::istreambuf_iterator<char>{}};
  while (state.KeepRunning()) {
    run(source);
  }
}
BENCHMARK(sum);

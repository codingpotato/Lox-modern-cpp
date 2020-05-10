#include <benchmark/benchmark.h>

#include <fstream>
#include <sstream>
#include <string>

#include "config.h"
#include "vm.h"

BENCHMARK_MAIN();

static std::string load_source(std::string file_path) noexcept {
  std::ifstream ifs(std::move(file_path));
  return std::string{std::istreambuf_iterator<char>{ifs},
                     std::istreambuf_iterator<char>{}};
}

static void run(std::string source) noexcept {
  std::ostringstream oss;
  lox::VM{oss}.interpret(std::move(source));
}

#define LOX_BENCHMARK(name)                                             \
  static void name(benchmark::State& state) {                           \
    auto source = load_source(EXAMPLES_DIR "/benchmark/" #name ".lox"); \
    while (state.KeepRunning()) {                                       \
      run(source);                                                      \
    }                                                                   \
  }                                                                     \
  BENCHMARK(name);

LOX_BENCHMARK(equality)
LOX_BENCHMARK(fib)
LOX_BENCHMARK(sum)

#include <benchmark/benchmark.h>

#include "compiler.h"
#include "scanner.h"
#include "virtual_machine.h"

static void sum(benchmark::State& state) {
  std::string source{R"(
var sum = 0.0;
var i = 0;
while (i < 10000000) {
    sum = sum + i;
    i = i + 1;
}
print sum;
)"};
  while (state.KeepRunning()) {
    lox::scanner scanner{source};
    auto chunk = lox::compiler{}.compile(scanner.scan());
    std::ostringstream oss;
    lox::virtual_machine{oss}.interpret(chunk);
  }
}
BENCHMARK(sum);

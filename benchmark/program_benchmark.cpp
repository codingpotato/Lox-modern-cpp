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
    std::ostringstream oss;
    lox::virtual_machine vm{oss};
    lox::compiler{vm}.compile(scanner.scan());
    vm.interpret();
  }
}
BENCHMARK(sum);

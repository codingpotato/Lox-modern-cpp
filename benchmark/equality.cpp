#include <benchmark/benchmark.h>

#include "helper.h"

static void expression_statement(benchmark::State& state) {
  const std::string source{R"(
var i = 0;
while (i < 10000000) {
  i = i + 1;

  1; 1; 1; 2; 1; nil; 1; "str"; 1; true;
  nil; nil; nil; 1; nil; "str"; nil; true;
  true; true; true; 1; true; false; true; "str"; true; nil;
  "str"; "str"; "str"; "stru"; "str"; 1; "str"; nil; "str"; true;
}
)"};
  while (state.KeepRunning()) {
    run(source);
  }
}
BENCHMARK(expression_statement);

static void equality_statement(benchmark::State& state) {
  const std::string source{R"(
var i = 0;
while (i < 10000000) {
  i = i + 1;

  1 == 1; 1 == 2; 1 == nil; 1 == "str"; 1 == true;
  nil == nil; nil == 1; nil == "str"; nil == true;
  true == true; true == 1; true == false; true == "str"; true == nil;
  "str" == "str"; "str" == "stru"; "str" == 1; "str" == nil; "str" == true;
}
)"};
  while (state.KeepRunning()) {
    run(source);
  }
}
BENCHMARK(equality_statement);

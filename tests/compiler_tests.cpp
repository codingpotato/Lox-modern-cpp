#include <doctest/doctest.h>

#include <ostream>
#include <string>

#include "compiler.h"
#include "virtual_machine.h"

inline std::string compile(const std::string& message,
                           std::string source) noexcept {
  lox::scanner scanner{std::move(source)};
  std::ostringstream oss;
  lox::virtual_machine vm{oss};
  auto function = lox::compiler{vm}.compile(scanner.scan());
  return function->code.repr(message);
}

TEST_CASE("primary") {
  const std::string source{R"(1; 2; nil;
  true; false; "str";
)"};
  const std::string expected = R"(== primary ==
0000    1 op_constant 1.000000
0001    | op_pop
0002    | op_constant 2.000000
0003    | op_pop
0004    | op_nil
0005    | op_pop
0006    2 op_true
0007    | op_pop
0008    | op_false
0009    | op_pop
0010    | op_constant "str"
0011    | op_pop
0012    3 op_return
)";
  CHECK_EQ(compile("primary", source), expected);
}

TEST_CASE("function call") {
  const std::string source{R"(
fun a() { b(); }
fun b() { c(); }
fun c() {
  c("too", "many");
}
a();
)"};
  const std::string expected = R"(== primary ==
0000    2 op_constant <function: a>
0001    | op_define_global a
0002    3 op_constant <function: b>
0003    | op_define_global b
0004    6 op_constant <function: c>
0005    | op_define_global c
0006    7 op_get_global a
0007    | op_call a
0008    | op_pop
0009    8 op_return
)";
  CHECK_EQ(compile("primary", source), expected);
}

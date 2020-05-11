#include <doctest/doctest.h>

#include <string>

#include "config.h"
#include "helper.h"

#define LOX_TEST_CASE(filename)                                       \
  TEST_CASE("lox: " filename) {                                       \
    auto [source, expected] = load(EXAMPLES_DIR "/" filename ".lox"); \
    REQUIRE_EQ(run(source), expected);                                \
  }

LOX_TEST_CASE("assignment/associativity")
LOX_TEST_CASE("assignment/global")
LOX_TEST_CASE("assignment/grouping")
LOX_TEST_CASE("assignment/infix_operator")
LOX_TEST_CASE("assignment/local")
LOX_TEST_CASE("assignment/prefix_operator")
LOX_TEST_CASE("assignment/syntax")
LOX_TEST_CASE("assignment/undefined")

LOX_TEST_CASE("block/empty")
LOX_TEST_CASE("block/scope")

LOX_TEST_CASE("bool/equality")
LOX_TEST_CASE("bool/not")

LOX_TEST_CASE("call/bool")
LOX_TEST_CASE("call/nil")
LOX_TEST_CASE("call/num")
LOX_TEST_CASE("call/string")

LOX_TEST_CASE("closure/assign_to_closure")
LOX_TEST_CASE("closure/assign_to_shadowed_later")
LOX_TEST_CASE("closure/close_over_function_parameter")
LOX_TEST_CASE("closure/close_over_later_variable")
LOX_TEST_CASE("closure/closed_closure_in_function")
LOX_TEST_CASE("closure/nested_closure")
LOX_TEST_CASE("closure/open_closure_in_function")
LOX_TEST_CASE("closure/reference_closure_multiple_times")
LOX_TEST_CASE("closure/reuse_closure_slot")
LOX_TEST_CASE("closure/shadow_closure_with_local")
LOX_TEST_CASE("closure/unused_closure")
LOX_TEST_CASE("closure/unused_later_closure")

LOX_TEST_CASE("comments/line_at_eof")
LOX_TEST_CASE("comments/only_line_comment")
LOX_TEST_CASE("comments/only_line_comment_and_line")
LOX_TEST_CASE("comments/unicode")

LOX_TEST_CASE("expressions/evaluate")

LOX_TEST_CASE("for/class_in_body")
LOX_TEST_CASE("for/closure_in_body")
LOX_TEST_CASE("for/fun_in_body")
LOX_TEST_CASE("for/return_closure")
LOX_TEST_CASE("for/return_inside")
LOX_TEST_CASE("for/scope")
LOX_TEST_CASE("for/statement_condition")
LOX_TEST_CASE("for/statement_increment")
LOX_TEST_CASE("for/statement_initializer")
LOX_TEST_CASE("for/syntax")
LOX_TEST_CASE("for/var_in_body")

LOX_TEST_CASE("function/body_must_be_block")
LOX_TEST_CASE("function/empty_body")
LOX_TEST_CASE("function/extra_arguments")
LOX_TEST_CASE("function/local_mutual_recursion")
LOX_TEST_CASE("function/local_recursion")
LOX_TEST_CASE("function/missing_arguments")
LOX_TEST_CASE("function/missing_comma_in_parameters")
LOX_TEST_CASE("function/mutual_recursion")
LOX_TEST_CASE("function/parameters")
LOX_TEST_CASE("function/print")
LOX_TEST_CASE("function/recursion")
LOX_TEST_CASE("function/too_many_arguments")
LOX_TEST_CASE("function/too_many_parameters")

LOX_TEST_CASE("if/class_in_else")
LOX_TEST_CASE("if/class_in_then")
LOX_TEST_CASE("if/dangling_else")
LOX_TEST_CASE("if/else")
LOX_TEST_CASE("if/fun_in_else")
LOX_TEST_CASE("if/fun_in_then")
LOX_TEST_CASE("if/if")
LOX_TEST_CASE("if/truth")
LOX_TEST_CASE("if/var_in_else")
LOX_TEST_CASE("if/var_in_then")

LOX_TEST_CASE("limit/loop_too_large")
LOX_TEST_CASE("limit/no_reuse_constants")
LOX_TEST_CASE("limit/stack_overflow")
LOX_TEST_CASE("limit/too_many_constants")
LOX_TEST_CASE("limit/too_many_locals")
LOX_TEST_CASE("limit/too_many_upvalues")

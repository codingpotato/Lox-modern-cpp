#include <doctest/doctest.h>

#include <string>

#include "helper.h"

TEST_CASE("bool: equality") {
  const std::string source{R"(
print true == true;
print true == false;
print false == true;
print false == false;
print true == 1;
print false == 0;
print true == "true";
print false == "false";
print false == "";
print true != true;
print true != false;
print false != true;
print false != false;
print true != 1;
print false != 0;
print true != "true";
print false != "false";
print false != "";
)"};
  const std::string expected{R"(true
false
false
true
false
false
false
false
false
false
true
true
false
true
true
true
true
true
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("bool: not") {
  const std::string source{R"(
print !true;
print !false;
print !!true;
)"};
  const std::string expected{R"(false
true
true
)"};
  CHECK_EQ(run(source), expected);
}

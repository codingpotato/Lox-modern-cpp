#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("string: error after multiline") {
  const std::string source{R"(
var a = "1
2
3
";
err;
)"};
  const std::string expected{R"(Undefined variable 'err'.
[line 0005] in <script>
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("string: literals") {
  const std::string source{R"raw(
print "(" + "" + ")";
print "a string";
print "A~¶Þॐஃ";
)raw"};
  const std::string expected{R"(()
a string
A~¶Þॐஃ
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("string: multiline") {
  const std::string source{R"(
var a = "1
2
3";
print a;
)"};
  const std::string expected{R"(1
2
3
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("string: unterminated") {
  const std::string source{R"(
"this string has no close quote
)"};
  const std::string expected{
      R"([line 3] Error at '"this string has no close quote
': Unterminated string.
)"};
  CHECK_EQ(run(source), expected);
}

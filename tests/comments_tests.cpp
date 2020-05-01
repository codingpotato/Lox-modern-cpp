#include <doctest/doctest.h>

#include <string>

#include "helper.h"

TEST_CASE("comments: line at eof") {
  const std::string source{R"(
print "ok";
// comment
)"};
  const std::string expected{R"(ok
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("comments: only line comment") {
  SUBCASE("only line comment") {
    const std::string source{R"(
// comment
)"};
    const std::string expected{""};
    CHECK_EQ(run(source), expected);
  }

  SUBCASE("only line comment and line") {
    const std::string source{R"(
// comment

)"};
    const std::string expected{""};
    CHECK_EQ(run(source), expected);
  }
}

TEST_CASE("comments: unicode") {
  const std::string source{R"(
// Unicode characters are allowed in comments.
//
// Latin 1 Supplement: £§¶ÜÞ
// Latin Extended-A: ĐĦŋœ
// Latin Extended-B: ƂƢƩǁ
// Other stuff: ឃᢆ᯽₪ℜ↩⊗┺░
// Emoji: ☃☺♣
print "ok"; // expect: ok
)"};
  const std::string expected{R"(ok
)"};
  CHECK_EQ(run(source), expected);
}

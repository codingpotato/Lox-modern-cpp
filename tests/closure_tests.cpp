#include <doctest/doctest.h>

#include "helper.h"

TEST_CASE("assign to closure") {
  std::string source{R"(
var f;
var g;
{
  var local = "local";
  fun f_() {
    print local;
    local = "after f";
    print local;
  }
  f = f_;
  fun g_() {
    print local;
    local = "after g";
    print local;
  }
  g = g_;
}
f();
g();
)"};
  std::string expected{R"("local"
"after f"
"after f"
"after g"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("assign to shadowed later") {
  std::string source{R"(
var a = "global";
{
  fun assign() {
    a = "assigned";
  }
  var a = "inner";
  assign();
  print a;
}
print a;
)"};
  std::string expected{R"("inner"
"assigned"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("close over function parameter") {
  std::string source{R"(
var f;
fun foo(param) {
  fun f_() {
    print param;
  }
  f = f_;
}
foo("param");
f();
)"};
  std::string expected{R"("param"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("close over later variable") {
  std::string source{R"(
fun f() {
  var a = "a";
  var b = "b";
  fun g() {
    print b;
    print a;
  }
  g();
}
f();
)"};
  std::string expected{R"("b"
"a"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("closed closure in function") {
  std::string source{R"(
var f;
{
  var local = "local";
  fun f_() {
    print local;
  }
  f = f_;
}
f();
)"};
  std::string expected{R"("local"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("nested closure") {
  std::string source{R"(
var f;
fun f1() {
  var a = "a";
  fun f2() {
    var b = "b";
    fun f3() {
      var c = "c";
      fun f4() {
        print a;
        print b;
        print c;
      }
      f = f4;
    }
    f3();
  }
  f2();
}
f1();
f();
)"};
  std::string expected{R"("a"
"b"
"c"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("open closure in function") {
  std::string source{R"(
{
  var local = "local";
  fun f() {
    print local;
  }
  f();
}
)"};
  std::string expected{R"("local"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("reference closure multiple times") {
  std::string source{R"(
var f;
{
  var a = "a";
  fun f_() {
    print a;
    print a;
  }
  f = f_;
}
f();
)"};
  std::string expected{R"("a"
"a"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("reuse closure slot") {
  std::string source{R"(
{
  var f;
  {
    var a = "a";
    fun f_() { print a; }
    f = f_;
  }
  {
    var b = "b";
    f();
  }
}
)"};
  std::string expected{R"("a"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("shadow closure with local") {
  std::string source{R"(
{
  var foo = "closure";
  fun f() {
    {
      print foo;
      var foo = "shadow";
      print foo;
    }
    print foo;
  }
  f();
}
)"};
  std::string expected{R"("closure"
"shadow"
"closure"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("unused closure") {
  std::string source{R"(
{
  var a = "a";
  if (false) {
    fun foo() { a; }
  }
}
print "ok";
)"};
  std::string expected{R"("ok"
)"};
  CHECK_EQ(run(source), expected);
}

TEST_CASE("unused later closure") {
  std::string source{R"(
var closure;
{
  var a = "a";
  {
    var b = "b";
    fun returnA() {
      return a;
    }
    closure = returnA;
    if (false) {
      fun returnB() {
        return b;
      }
    }
  }
  print closure();
}
)"};
  std::string expected{R"("a"
)"};
  CHECK_EQ(run(source), expected);
}

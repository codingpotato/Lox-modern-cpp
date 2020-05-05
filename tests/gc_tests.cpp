#include <doctest/doctest.h>

#include <vector>

#include "gc.h"
#include "hash_table.h"
#include "list.h"
#include "object.h"
#include "stack.h"
#include "value.h"

constexpr size_t max_size = 10;

struct Heap_mockup {
  using Upvalue_list = lox::List<lox::Upvalue, false>;

  struct Delegate {
    virtual ~Delegate() noexcept = default;
    virtual void collect_garbage() noexcept = 0;
  };

  const Upvalue_list& get_open_upvalues() const noexcept { return upvalues; }

  void set_delegate(Delegate&) const noexcept {}

  void sweep() noexcept {}

  Upvalue_list upvalues;
};

struct Call_frame {
  lox::Closure* closure;
};
using Call_frame_stack = lox::Stack<Call_frame, max_size>;

using Value_stack = lox::Stack<lox::Value, max_size>;

struct Compiler {
  template <typename Visitor>
  void for_each_func(Visitor&& visitor) const noexcept {
    for (auto func : functions) {
      visitor(func);
    }
  }

  std::vector<lox::Function*> functions;
};

TEST_CASE("gc") {
  Heap_mockup heap;
  lox::Hash_table globals;
  Value_stack stack;
  Call_frame_stack call_frames;
  Compiler compiler;

  lox::String string{"string"};
  stack.push(&string);

  lox::String key{"key"};
  lox::String value{"value"};
  globals.insert(&key, &value);

  lox::Function func;
  lox::String func_name{"func"};
  func.name = &func_name;
  func.upvalue_count = 1;

  lox::Upvalue upvalue{nullptr};
  lox::String closed{"closed"};
  upvalue.closed = &closed;

  lox::Closure closure{&func};
  closure.get_upvalues()[0] = &upvalue;
  call_frames.push(&closure);

  lox::Upvalue open_upvalue{nullptr};
  lox::String open_closed{"open_closed"};
  open_upvalue.closed = &open_closed;
  heap.upvalues.insert(&open_upvalue);

  lox::Function compile_func;
  lox::String compile_func_name{"compile_func"};
  func.name = &compile_func_name;

  compiler.functions.push_back(&compile_func);

  REQUIRE(!string.is_marked);
  REQUIRE(!key.is_marked);
  REQUIRE(!value.is_marked);
  REQUIRE(!func.is_marked);
  REQUIRE(!upvalue.is_marked);
  REQUIRE(!closed.is_marked);
  REQUIRE(!closure.is_marked);
  REQUIRE(!open_upvalue.is_marked);
  REQUIRE(!open_closed.is_marked);
  REQUIRE(!compile_func.is_marked);

  lox::GC<Heap_mockup, lox::Hash_table, Value_stack, Call_frame_stack, Compiler>
      gc{heap, globals, stack, call_frames, compiler};
  gc.collect_garbage();

  REQUIRE(string.is_marked);
  REQUIRE(key.is_marked);
  REQUIRE(value.is_marked);
  REQUIRE(func.is_marked);
  REQUIRE(upvalue.is_marked);
  REQUIRE(closed.is_marked);
  REQUIRE(closure.is_marked);
  REQUIRE(open_upvalue.is_marked);
  REQUIRE(open_closed.is_marked);
  REQUIRE(compile_func.is_marked);
}

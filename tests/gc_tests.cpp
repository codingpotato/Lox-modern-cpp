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
  using Upvalue_list = lox::List<lox::Upvalue>;

  struct Delegate {
    virtual ~Delegate() noexcept = default;
    virtual void collect_garbage() noexcept = 0;
  };

  const Upvalue_list& get_open_upvalues() const noexcept { return upvalues; }

  void set_delegate(Delegate&) const noexcept {}

  void sweep() noexcept {}

  void erase_unmarked_strings() noexcept {}

  Upvalue_list upvalues;
};

struct Call_frame {
  lox::Closure* closure;
};
using Call_frame_stack = lox::Stack<Call_frame, max_size>;

using Value_stack = lox::Stack<lox::Value, max_size>;

struct Compiler {
  template <typename Visitor>
  void for_each_func(Visitor&&) const noexcept {}
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

  lox::GC<Heap_mockup, lox::Hash_table, Value_stack, Call_frame_stack, Compiler>
      gc{heap, globals, stack, call_frames, compiler};
  gc.collect_garbage();

  REQUIRE(string.is_marked);
  REQUIRE(key.is_marked);
  REQUIRE(value.is_marked);
}

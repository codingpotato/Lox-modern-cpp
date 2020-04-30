#ifndef LOX_OBJECT_H
#define LOX_OBJECT_H

#include <string>
#include <vector>

#include "chunk.h"
#include "contract.h"
#include "exception.h"
#include "type_list.h"

namespace lox {

class String;
class Function;
class Native_func;
class Upvalue;
class Closure;

class Object {
 public:
  using Types = Type_list<String, Function, Native_func, Upvalue, Closure>;
  template <typename T>
  constexpr static size_t id_of = Index_of<T, Types>::value;

  explicit Object(size_t id) noexcept : id{id} { ENSURES(id < Types::size); }
  Object(const Object&) noexcept = delete;
  Object(Object&&) noexcept = delete;
  Object& operator=(const Object&) noexcept = delete;
  Object& operator=(Object&&) noexcept = delete;
  virtual ~Object() noexcept = default;

  template <typename T>
  bool is() const noexcept {
    return id == id_of<T>;
  }

  template <typename T>
  const T* as() const noexcept {
    ENSURES(is<T>());
    return static_cast<const T*>(this);
  }

  template <typename T>
  T* as() noexcept {
    return const_cast<T*>(std::as_const(*this).as<T>());
  }

  virtual std::string to_string(bool = false) const noexcept = 0;

  Object* next = nullptr;
  bool is_marked = false;

 private:
  size_t id;
};

class String : public Object {
 public:
  static uint32_t hash_from(const std::string& string) noexcept {
    uint32_t hash = 2166136261u;
    for (const auto ch : string) {
      hash ^= ch;
      hash *= 16777619;
    }
    return hash;
  }

  String(std::string str) noexcept
      : Object{id_of<String>},
        string{std::move(str)},
        hash{hash_from(string)} {}

  const std::string& get_string() const noexcept { return string; }
  uint32_t get_hash() const noexcept { return hash; }

  std::string to_string(bool = false) const noexcept override { return string; }

  friend bool operator==(const String& lhs, const String& rhs) noexcept {
    return lhs.hash == rhs.hash && lhs.string == rhs.string;
  }

 private:
  std::string string;
  uint32_t hash;
};

class Function : public Object {
 public:
  Function() noexcept : Object{id_of<Function>} {}

  const Chunk& get_chunk() const noexcept { return chunk; }
  Chunk& get_chunk() noexcept { return chunk; }
  size_t get_arity() const noexcept { return arity; }
  const String* get_name() const noexcept { return name; }
  String* get_name() noexcept { return name; }

  void inc_arity() noexcept { ++arity; }

  std::string to_string(bool verbose = false) const noexcept override {
    const std::string message =
        name ? "<function: " + name->get_string() + ">" : "<script>";
    return verbose ? ::lox::to_string(chunk, message, 1) : message;
  }

  String* name = nullptr;
  size_t upvalue_count = 0;

 private:
  Chunk chunk;
  size_t arity = 0;
};

class Native_func : public Object {
 public:
  using Func = Value (*)(int arg_count, Value* args) noexcept;

  Native_func(Func func) noexcept : Object{id_of<Native_func>}, func{func} {}

  Value operator()(int arg_count, Value* args) const noexcept {
    return (*func)(arg_count, args);
  }

  std::string to_string(bool = false) const noexcept override {
    return "<native func>";
  }

 private:
  Func func;
};

class Upvalue : public Object {
 public:
  explicit Upvalue(Value* location) noexcept
      : Object{id_of<Upvalue>}, location{location} {}

  std::string to_string(bool = false) const noexcept override {
    return "upvalue";
  }

  Value* location;
  Value closed;
  Upvalue* next = nullptr;
};

class Closure : public Object {
 public:
  using Upvalue_vector = std::vector<Upvalue*>;

  Closure(Function* func) noexcept
      : Object{id_of<Closure>},
        func{func},
        upvalues{func->upvalue_count, nullptr} {}

  const Function* get_func() const noexcept { return func; }
  Function* get_func() noexcept { return func; }
  const Upvalue_vector& get_upvalues() const noexcept { return upvalues; }
  Upvalue_vector& get_upvalues() noexcept { return upvalues; }

  std::string to_string(bool verbose = false) const noexcept override {
    return func->to_string(verbose);
  }

 private:
  Function* func;
  Upvalue_vector upvalues;
};

}  // namespace lox

#endif

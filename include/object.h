#ifndef LOX_OBJECT_H
#define LOX_OBJECT_H

#include <string>

#include "chunk.h"
#include "contract.h"
#include "exception.h"
#include "type_list.h"

namespace lox {

struct Function;
struct Native_func;
struct String;

struct Object {
  using Types = type_list<Function, Native_func, String>;
  template <typename T>
  constexpr static size_t id = index_of<T, Types>::value;

  explicit Object(size_t id) noexcept : id_{id} {}
  virtual ~Object() = default;

  template <typename T>
  bool is() const noexcept {
    return id_ == id<T>;
  }

  template <typename T>
  T* as() noexcept {
    ENSURES(is<T>());
    return reinterpret_cast<T*>(this);
  }

  virtual std::string to_string(bool = false) noexcept = 0;

  Object* next = nullptr;

 private:
  size_t id_;
};

struct String : Object {
  static uint32_t hash(const std::string& str) noexcept {
    uint32_t hash = 2166136261u;
    for (const auto ch : str) {
      hash ^= ch;
      hash *= 16777619;
    }
    return hash;
  }

  String(std::string str) noexcept
      : Object{id<String>}, str_{std::move(str)}, hash_{hash(str_)} {}

  const std::string& string() const noexcept { return str_; }
  uint32_t hash() const noexcept { return hash_; }

  std::string to_string(bool = false) noexcept override { return str_; }

  friend bool operator==(const String& lhs, const String& rhs) noexcept {
    return lhs.hash_ == rhs.hash_ && lhs.str_ == rhs.str_;
  }

 private:
  std::string str_;
  uint32_t hash_;
};

struct Function : Object {
  explicit Function(const String* str) noexcept
      : Object{id<Function>}, name_{str} {}

  size_t arity() const noexcept { return arity_; }
  chunk& code() noexcept { return code_; }
  const String* name() const noexcept { return name_; }

  void increase_arity() noexcept { ++arity_; }

  std::string to_string(bool verbose = false) noexcept override {
    const std::string message =
        name_ ? "<function: " + name_->string() + ">" : "<script>";
    return verbose ? ::lox::to_string(code_, message, 1) : message;
  }

 private:
  size_t arity_ = 0;
  chunk code_;
  const String* name_;
};

struct Native_func : Object {
  using Func = Value (*)(int arg_count, Value* args) noexcept;

  Native_func(Func func) noexcept : Object{id<Native_func>}, func_{func} {}

  Value operator()(int arg_count, Value* args) const noexcept {
    return (*func_)(arg_count, args);
  }

  std::string to_string(bool = false) noexcept override {
    return "<native func>";
  }

 private:
  Func func_;
};

}  // namespace lox

#endif

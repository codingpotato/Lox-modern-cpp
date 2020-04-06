#ifndef LOX_OBJECT_H
#define LOX_OBJECT_H

#include <string>

#include "chunk.h"
#include "contract.h"
#include "exception.h"
#include "type_list.h"

namespace lox {

struct function;
struct string;

struct object {
  using types = type_list<function, string>;
  template <typename T>
  constexpr static size_t id = index_of<T, types>::value;

  explicit object(size_t id) noexcept : id_{id} {}
  virtual ~object() = default;

  template <typename T>
  bool is() const noexcept {
    return id_ == id<T>;
  }

  template <typename T>
  const T* as() const noexcept {
    ENSURES(is<T>());
    return reinterpret_cast<const T*>(this);
  }

  virtual std::string to_string(bool = false) const noexcept = 0;

  object* next = nullptr;

 private:
  size_t id_;
};

struct string : object {
  static uint32_t hash(const std::string& str) noexcept {
    uint32_t hash = 2166136261u;
    for (const auto ch : str) {
      hash ^= ch;
      hash *= 16777619;
    }
    return hash;
  }

  string(std::string str) noexcept
      : object{id<string>}, str_{std::move(str)}, hash_{hash(str_)} {}

  const std::string& std_string() const noexcept { return str_; }
  uint32_t hash() const noexcept { return hash_; }

  std::string to_string(bool = false) const noexcept override { return str_; }

  friend bool operator==(const string& lhs, const string& rhs) noexcept {
    return lhs.hash_ == rhs.hash_ && lhs.str_ == rhs.str_;
  }

 private:
  std::string str_;
  uint32_t hash_;
};

struct function : object {
  function() noexcept : object{id<function>} {}
  explicit function(const string* n) noexcept : object{id<function>}, name{n} {}

  std::string to_string(bool verbose = false) const noexcept override {
    std::string message =
        name ? "<function: " + name->std_string() + ">" : "<script>";
    return verbose ? ::lox::to_string(code, message, 1) : message;
  }

  int arity = 0;
  chunk code;
  const string* name = nullptr;
};

}  // namespace lox

#endif

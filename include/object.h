#ifndef LOX_OBJECT_H
#define LOX_OBJECT_H

#include <string>

#include "chunk.h"
#include "contract.h"
#include "exception.h"

namespace lox {

struct object {
  enum class type { string, function };

  object(type t) noexcept : type_{t} {}
  virtual ~object() = default;

  template <typename T>
  bool is() const noexcept;

  template <typename T>
  const T* as() const;

  virtual std::string repr() const noexcept = 0;

  object* next = nullptr;

 private:
  type type_;
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
      : object{type::string}, str_{std::move(str)}, hash_{hash(str_)} {}

  uint32_t hash() const noexcept { return hash_; }

  operator const std::string&() const noexcept { return str_; }

  std::string repr() const noexcept override { return str_; }

  friend bool operator==(const string& lhs, const string& rhs) noexcept {
    return lhs.hash_ == rhs.hash_ && lhs.str_ == rhs.str_;
  }

 private:
  std::string str_;
  uint32_t hash_;
};

struct function : object {
  std::string repr() const noexcept override {
    std::string name = *name_;
    if (name.empty()) {
      return "<script>";
    }
    return std::string{"<function: "} + name + " > ";
  }

 private:
  // int arity_;
  // chunk code_;
  string* name_;
};

template <typename T>
inline bool object::is() const noexcept {
  if constexpr (std::is_same_v<T, string>) {
    return type_ == type::string;
  }
  if constexpr (std::is_same_v<T, function>) {
    return type_ == type::function;
  }
  throw internal_error{"Unknown object type."};
}

template <typename T>
inline const T* object::as() const {
  if constexpr (std::is_same_v<T, string>) {
    ENSURES(type_ == type::string);
    return reinterpret_cast<const T*>(this);
  }
  if constexpr (std::is_same_v<T, function>) {
    ENSURES(type_ == type::function);
    return reinterpret_cast<const T*>(this);
  }
  throw internal_error{"Unknown object type."};
}

}  // namespace lox

#endif

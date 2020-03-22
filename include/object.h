#ifndef LOX_OBJECT_H
#define LOX_OBJECT_H

#include <string>
#include <variant>

#include "contract.h"

namespace lox {

struct string {
  string(std::string str) noexcept : str_{std::move(str)}, hash_{hash(str_)} {}
  uint32_t hash() const noexcept { return hash_; }

  friend bool operator==(const string& lhs, const string& rhs) noexcept {
    return lhs.str_ == rhs.str_;
  }

 private:
  static uint32_t hash(const std::string& str) noexcept {
    uint32_t hash = 2166136261u;
    for (const auto ch : str) {
      hash ^= ch;
      hash *= 16777619;
    }
    return hash;
  }

  std::string str_;
  uint32_t hash_;
};

struct object {
  object(string str) noexcept : storage_{std::move(str)} {}

  uint32_t hash() const noexcept { return std::get<string>(storage_).hash(); }

  template <typename T>
  const T& as() const noexcept {
    return std::get<T>(storage_);
  }

  friend bool operator==(const object& lhs, const object& rhs) noexcept {
    if (std::holds_alternative<string>(lhs.storage_) &&
        std::holds_alternative<string>(rhs.storage_)) {
      return lhs.as<string>() == rhs.as<string>();
    }
    return false;
  }

 private:
  using storage = std::variant<string>;

  storage storage_;
};

}  // namespace lox

#endif

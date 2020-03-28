#ifndef LOX_OBJECT_H
#define LOX_OBJECT_H

#include <string>

namespace lox {

struct object {
  object* next = nullptr;
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

  string(std::string str) noexcept : str_{std::move(str)}, hash_{hash(str_)} {}

  uint32_t hash() const noexcept { return hash_; }

  operator const std::string&() const noexcept { return str_; }

  friend bool operator==(const string& lhs, const string& rhs) noexcept {
    return lhs.hash_ == rhs.hash_ && lhs.str_ == rhs.str_;
  }

 private:
  std::string str_;
  uint32_t hash_;
};

}  // namespace lox

#endif

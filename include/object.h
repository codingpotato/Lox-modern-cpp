#ifndef LOX_OBJECT_H
#define LOX_OBJECT_H

#include <string>

#include "contract.h"
#include "exception.h"
#include "type_list.h"

namespace lox {

struct object {
  struct string_t;
  using types = type_list<string_t>;

  struct string_t {
    constexpr static std::size_t id = index_of<string_t, types>::value;
  };

  object(std::size_t t) noexcept : type_{t} {}
  virtual ~object() = default;

  template <typename T>
  bool is() const noexcept;

  template <typename T>
  const T* as() const;

  object* next = nullptr;

 private:
  std::size_t type_;
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
      : object{object::string_t::id}, str_{std::move(str)}, hash_{hash(str_)} {}

  uint32_t hash() const noexcept { return hash_; }

  operator const std::string&() const noexcept { return str_; }

  friend bool operator==(const string& lhs, const string& rhs) noexcept {
    return lhs.hash_ == rhs.hash_ && lhs.str_ == rhs.str_;
  }

 private:
  std::string str_;
  uint32_t hash_;
};

template <typename T>
inline bool object::is() const noexcept {
  if constexpr (std::is_same_v<T, string>) {
    return type_ == string_t::id;
  }
  throw internal_error{""};
}

template <typename T>
inline const T* object::as() const {
  if constexpr (std::is_same_v<std::decay_t<T>, string>) {
    ENSURES(type_ == string_t::id);
    return reinterpret_cast<const string*>(this);
  }
  throw internal_error{"Unknown object type"};
}

}  // namespace lox

#endif

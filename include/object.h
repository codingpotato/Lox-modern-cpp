#ifndef LOX_OBJECT_H
#define LOX_OBJECT_H

#include <string>
#include <variant>

#include "contract.h"

namespace lox {

struct object {
  explicit object(std::string str) noexcept
      : type_{string}, string_{std::move(str)} {}

  ~object() noexcept {
    switch (type_) {
      case string:
        string_.~basic_string();
        break;
    }
  }

  template <typename T>
  const std::string& as() const noexcept {
    ENSURES(type_ == string);
    return string_;
  }

 private:
  enum type_t { string };

  type_t type_;
  union {
    std::string string_;
  };
};

}  // namespace lox

#endif

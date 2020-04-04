#ifndef LOX_HEAP_H
#define LOX_HEAP_H

#include <string>

#include "hash_table.h"
#include "list.h"
#include "object.h"
#include "value.h"

namespace lox {

struct heap {
  template <typename T, typename... Args>
  T* make_object(Args&&... args) noexcept {
    auto obj = new T{std::forward<Args>(args)...};
    objects.insert(obj);
    return obj;
  }

  string* make_string(std::string str) noexcept {
    auto obj = strings.find_string(str);
    if (!obj) {
      obj = new string{std::move(str)};
      objects.insert(obj);
      strings.insert(obj, value{true});
    }
    return obj;
  }

 private:
  list objects;
  hash_table strings;
};

}  // namespace lox

#endif

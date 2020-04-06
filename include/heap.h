#ifndef LOX_HEAP_H
#define LOX_HEAP_H

#include <string>

#include "hash_table.h"
#include "list.h"
#include "object.h"
#include "value.h"

namespace lox {

struct Heap {
  template <typename T, typename... Args>
  T* make_object(Args&&... args) noexcept {
    auto obj = new T{std::forward<Args>(args)...};
    objects_.insert(obj);
    return obj;
  }

  String* make_string(std::string str) noexcept {
    auto obj = strings_.find_string(str);
    if (!obj) {
      obj = new String{std::move(str)};
      objects_.insert(obj);
      strings_.insert(obj, true);
    }
    return obj;
  }

 private:
  List objects_;
  Hash_table strings_;
};

}  // namespace lox

#endif

#ifndef LOX_HEAP_H
#define LOX_HEAP_H

#include <string>

#include "hash_table.h"
#include "list.h"
#include "object.h"
#include "value.h"

namespace lox {

struct heap {
  string* add_string(std::string str) noexcept {
    auto obj = strings_.find_string(str);
    if (!obj) {
      obj = new string{std::move(str)};
      objects_.insert(obj);
      strings_.insert(obj, value{true});
    }
    return obj;
  }

 private:
  list objects_;
  hash_table strings_;
};

}  // namespace lox

#endif

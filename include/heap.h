#ifndef LOX_HEAP_H
#define LOX_HEAP_H

#include <string>

#include "hash_table.h"
#include "list.h"
#include "object.h"
#include "value.h"

namespace lox {

class Heap {
 public:
  using Object_list = List<Object>;
  using Upvalue_list = List<Upvalue, false>;

  template <typename T, typename... Args>
  T* make_object(Args&&... args) noexcept {
    const auto obj = new T{std::forward<Args>(args)...};
    objects.insert(obj);
    return obj;
  }

  String* make_string(std::string str) noexcept {
    auto string = strings.find_string(str);
    if (!string) {
      string = new String{std::move(str)};
      objects.insert(string);
      strings.insert(string, true);
    }
    return string;
  }

  Upvalue* make_upvalue(Value* location) noexcept {
    Upvalue* previous = nullptr;
    auto it = open_upvalues.begin();
    while (it != open_upvalues.end() && it->location > location) {
      previous = it;
      ++it;
    }
    if (it != open_upvalues.end() && it->location == location) {
      return it;
    }
    auto upvalue = make_object<Upvalue>(location);
    open_upvalues.insert(upvalue, previous);
    return upvalue;
  }

  const Upvalue_list& get_open_upvalues() const noexcept {
    return open_upvalues;
  }

 private:
  Object_list objects;
  Hash_table strings;
  Upvalue_list open_upvalues;
};

}  // namespace lox

#endif

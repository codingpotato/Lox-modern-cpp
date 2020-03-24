#ifndef LOX_HEAP_H
#define LOX_HEAP_H

#include "hash_table.h"
#include "list.h"
#include "object.h"

namespace lox {

struct heap {
  void add_string(const std::string& str) noexcept {}

 private:
  list<refectoring::object> objects_;
  set<refectoring::string> strings_;
};

}  // namespace lox

#endif

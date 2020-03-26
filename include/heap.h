#ifndef LOX_HEAP_H
#define LOX_HEAP_H

#include "hash_table.h"

namespace lox {

struct heap {
  void add_string(const std::string&) noexcept {}

 private:
  hash_table objects_;
  hash_table strings_;
};

}  // namespace lox

#endif

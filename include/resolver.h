#ifndef LOX_RESOLVER_H
#define LOX_RESOLVER_H

#include <map>
#include <vector>

#include "cache.h"
#include "exception.h"
#include "types.h"

namespace lox {

struct resolve_info {
  int distance;
  int index;
};

class resolver_t {
 public:
  struct scope_t {
    void declear(const string& name) noexcept {
      variables.push_back(false);
      id_map.emplace(name, variables.size() - 1);
    }

    void define(const string& name) noexcept {
      Expect(id_map.find(name) != id_map.cend(), name + "must be decleared.");
      Expect(id_map[name] >= 0 &&
                 id_map[name] < static_cast<int>(variables.size()),
             "Id map is not correct.");
      variables[id_map[name]] = true;
    }

    int resolve(const string& name) {
      if (id_map.find(name) != id_map.cend()) {
        return id_map[name];
      }
      return -1;
    }

   private:
    using variable_vector = std::vector<bool>;

    std::map<string, int> id_map;
    variable_vector variables;
  };

  using scope_vector = std::vector<scope_t>;

  void begin_scope() { scopes.emplace_back(scope_t{}); }
  void end_scope() { scopes.pop_back(); }

  void declear(const string& name) {
    if (!scopes.empty()) {
      scopes.back().declear(name);
    }
  }

  void define(const string& name) noexcept {
    if (!scopes.empty()) {
      scopes.back().define(name);
    }
  }

  resolve_info resolve(const string& name) {
    for (int i = scopes.size() - 1; i >= 0; --i) {
      if (auto index = scopes[i].resolve(name); index >= 0) {
        return {static_cast<int>(scopes.size()) - 1 - i, index};
      }
    }
    throw parse_error{"Variable " + name + "not defined."};
  }

  scope_vector scopes;
};

}  // namespace lox

#endif

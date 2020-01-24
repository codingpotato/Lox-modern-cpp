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
  enum status { none, decleared, defined };

  struct scope_t {
    void declear(const string& name) noexcept {
      variables.push_back(decleared);
      id_map.emplace(name, variables.size() - 1);
    }

    void define(const string& name) noexcept {
      Expect(id_map.find(name) != id_map.cend());
      Expect(id_map[name] >= 0 &&
             id_map[name] < static_cast<int>(variables.size()));
      variables[id_map[name]] = defined;
    }

    status variable_status(const string& name) const noexcept {
      if (id_map.find(name) != id_map.cend()) {
        Expect(id_map.at(name) >= 0 &&
               id_map.at(name) < static_cast<int>(variables.size()));
        return variables[id_map.at(name)];
      }
      return none;
    }

    int resolve(const string& name) {
      if (id_map.find(name) != id_map.cend()) {
        return id_map[name];
      }
      return -1;
    }

   private:
    using variable_vector = std::vector<status>;

    std::map<string, int> id_map;
    variable_vector variables;
  };

  using scope_vector = std::vector<scope_t>;

  void begin_scope() { scopes.emplace_back(scope_t{}); }
  void end_scope() { scopes.pop_back(); }

  void declear(const string& name) {
    Expect(!scopes.empty());
    scopes.back().declear(name);
  }

  void define(const string& name) noexcept {
    Expect(!scopes.empty());
    scopes.back().define(name);
  }

  bool is_in_variable_declearation(const string& name) const noexcept {
    Expect(!scopes.empty());
    return scopes.back().variable_status(name) == decleared;
  }

  resolve_info resolve(const string& name) {
    for (int i = scopes.size() - 1; i >= 0; --i) {
      if (auto index = scopes[i].resolve(name); index >= 0) {
        return {static_cast<int>(scopes.size()) - 1 - i, index};
      }
    }
    throw parse_error{"Variable \"" + name + "\" not defined."};
  }

  scope_vector scopes;
};

}  // namespace lox

#endif

#ifndef LOX_RESOLVER_H
#define LOX_RESOLVER_H

#include <map>
#include <vector>

#include "cache.h"
#include "types.h"

namespace lox {

struct resolve_info {
  int distance;
  int index;
};

class resolver {
 public:
  /*void resolve_block(const statements& ss) {
    begin_scope();
    resolve(ss);
    end_scope();
  }

  void resolve(const statements& ss) {
    for (auto& s : ss) {
      resolve(s);
    }
  }

 private:
  void resolve(const statement& s) {
    s.visit(overloaded{[this](auto&& s) { resolve(s); }});
  }

  void resolve(const statement::block& bs) { resolve_block(bs.statements_); }

  void resolve(const statement::expr& es) { resolve(es.expr_); }

  void resolve(const statement::function& fun) {
    declear(fun.name_);
    define(fun.name_);
    resolve_fun(fun);
  };

  void resolve_fun(const statement::function& fun) {
    begin_scope();
    for (auto& param : fun.parameters_) {
      declear(param);
      define(param);
    }
    resolve(fun.body_);
    end_scope();
  };

  void resolve(const statement::if_else& is) {
    resolve(is.condition_);
    resolve_block(is.then_);
    resolve_block(is.else_);
  }

  void resolve(const statement::return_s& rs) { resolve(rs.value_); }

  void resolve(const statement::var& vs) {
    declear(vs.name_);
    resolve(vs.initializer_);
    define(vs.name_);
  }

  void resolve(const statement::while_loop& ws) {
    resolve(ws.condition_);
    resolve_block(ws.ss_);
  }

  void resolve(const expression& expr) { resolve(expr, expr.root()); }

  void resolve(const expression& expr, const expression::node& n) {
    n.visit(overloaded{[this, &expr](auto& e) { resolve(expr, e); }});
  }

  void resolve(const expression& expr, const expression::assignment& a) {
    resolve(expr, expr[a.v_index_]);
    resolve_local(a);
  }

  void resolve(const expression& expr, const expression::binary& b) {
    resolve(expr, expr[b.l_index_]);
    resolve(expr, expr[b.r_index_]);
  }

  void resolve(const expression& expr, const expression::call& c) {
    resolve(expr, expr[c.callee_]);
    for (auto& argument : c.arguments_) {
      resolve(expr, expr[argument]);
    }
  }

  void resolve(const expression& expr, const expression::group& g) {
    resolve(expr, expr[g.node_index_]);
  }

  void resolve(const expression&, const expression::literal&) {}

  void resolve(const expression& expr, const expression::unary& u) {
    resolve(expr, expr[u.node_index_]);
  }

  void resolve(const expression&, const expression::variable& v) {
    if (!scopes_.empty()) {
      if (auto& m = scopes_.back();
          m.find(v.name_) != m.end() && m[v.name_] == false) {
        throw runtime_error(
            "Cannot read local variable in its own initializer.");
      }
    }
    resolve_local(v);
  }*/

  using variable_definitions = cache<bool>;

  struct scope_t {
    void declear(const string& name) noexcept {
      varibles.add(false);
      variable_ids.emplace(name, variables.size() - 1);
    }

    void define(const string& name) noexcept { variable_ids[name] = true; }

    std::map<string, variable_definitions::index> variable_ids;
    variable_definitions varibles;
  };

  using scope_vector = std::vector<scope_t>;

  void begin_scope() { scopes.emplace_back(scope_t{}); }
  void end_scope() { scopes.pop_back(); }

  void declear(const string& name) {
    if (!scopes.empty()) {
      scopes.back().declear(name);
    }
  }

  void define(const string& name) {
    if (!scopes.empty()) {
      scopes.define(name);
    }
  }

  template <typename E>
  resolve_info resolve(const E& expr) {
    for (int i = scopes.size() - 1; i >= 0; --i) {
      if (auto& m = scopes[i]; m.find(expr.name) != m.end()) {
        expr.resolve_info_.env_distance_ = scopes_.size() - 1 - i;
        return;
      }
    }
  }

  scope_vector scopes;
};

}  // namespace lox

#endif

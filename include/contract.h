#ifndef LOX_CONTRACT_H
#define LOX_CONTRACT_H

#include <exception>
#include <sstream>
#include <string>

namespace lox {
namespace contract {

struct exception : public std::exception {
  exception(const char* rule, const char* filename, int line) noexcept {
    std::ostringstream ss;
    ss << "Condition '" << rule << "' violated at " << filename << ":" << line;
    message_ = ss.str();
  }
  virtual ~exception() {}

  const char* what() const noexcept override { return message_.c_str(); }

 private:
  std::string message_;
};

inline constexpr void assures(bool condition, const char* rule,
                              const char* filename, int line) {
  if (!condition) {
    throw exception(rule, filename, line);
  }
}

}  // namespace contract

}  // namespace lox

#ifdef NDEBUG
#define ASSURES(condition) (void)0;
#else
#define ASSURES(condition) \
  contract::assures(condition, #condition, __FILE__, __LINE__);
#endif

#define ENSURES(condition) ASSURES(condition)
#define EXPECTS(condition) ASSURES(condition)

#endif

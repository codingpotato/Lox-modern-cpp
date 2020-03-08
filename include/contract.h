#ifndef LOX_CONTRACT_H
#define LOX_CONTRACT_H

#include <exception>
#include <sstream>
#include <string>

namespace lox {
namespace contract {

struct exception : public std::exception {
 private:
  std::string message;

 public:
  exception(const char* rule, const char* filename, int line) noexcept {
    std::ostringstream ss;
    ss << "Condition '" << rule << "' violated at " << filename << ":" << line;
    message = ss.str();
  }
  virtual ~exception() {}

  const char* what() const noexcept override { return message.c_str(); }
};

inline constexpr void assures(bool condition, const char* rule,
                              const char* filename, int line) {
  if (!condition) {
    throw exception(rule, filename, line);
  }
}

};  // namespace contract

#ifdef NDEBUG
#define ASSURES(condition) (void)0;
#else
#define ASSURES(condition) \
  contract::assures(condition, #condition, __FILE__, __LINE__);
#endif

#define ENSURES(condition) ASSURES(condition)
#define EXPECTS(condition) ASSURES(condition)

}  // namespace lox

#endif

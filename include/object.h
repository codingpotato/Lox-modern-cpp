#ifndef LOX_OBJECT_H
#define LOX_OBJECT_H

#include <string>
#include <variant>

namespace lox {

namespace variant {

struct object {
  explicit object(std::string string) noexcept : storage{std::move(string)} {}

  template <typename T>
  const std::string& as() const noexcept {
    return std::get<std::string>(storage);
  }

 private:
  using storage_t = std::variant<std::string>;

  storage_t storage;
};

}  // namespace variant

using object = variant::object;

}  // namespace lox

#endif

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>

#include <fstream>
#include <iostream>

#include "compiler.h"
#include "scanner.h"
#include "virtual_machine.h"

namespace lox {

inline void run(const std::string &source) {
  try {
    scanner scanner{source};
    auto chunk = compiler{}.compile(scanner.scan());
    lox::virtual_machine{std::cout}.interpret(chunk);
  } catch (std::exception &e) {
    std::cout << e.what() << '\n';
  }
}

inline void repl() noexcept {}

inline void run_file(const std::string &filepath) {
  std::ifstream ifs(filepath);
  run(std::string((std::istreambuf_iterator<char>(ifs)),
                  (std::istreambuf_iterator<char>())));
}

inline int main(int argc, char *argv[]) noexcept {
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    run_file(argv[1]);
  } else {
    fprintf(stderr, "Usage: lox [path]\n");
  }
  return 0;
}

}  // namespace lox

int main(int argc, char *argv[]) {
  doctest::Context context;
  context.applyCommandLine(argc, argv);

  int res = context.run();
  if (context.shouldExit()) {
    return res;
  }

  int client_stuff_return_code = lox::main(argc, argv);
  return res + client_stuff_return_code;
}

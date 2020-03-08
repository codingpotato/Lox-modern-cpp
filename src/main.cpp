#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>

#include <fstream>
#include <iostream>

inline void repl() noexcept {}

inline void run_file(const std::string &) {}

inline int real_main(int argc, char *argv[]) noexcept {
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    run_file(argv[1]);
  } else {
    fprintf(stderr, "Usage: lox [path]\n");
  }
  return 0;
}

int main(int argc, char *argv[]) {
  doctest::Context context;
  context.applyCommandLine(argc, argv);

  int res = context.run();
  if (context.shouldExit()) {
    return res;
  }

  int client_stuff_return_code = real_main(argc, argv);
  return res + client_stuff_return_code;
}

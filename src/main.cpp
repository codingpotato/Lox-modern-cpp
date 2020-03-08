#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>

#include <fstream>
#include <iostream>

int real_main(int argc, char *[]) {
  if (argc < 3) {
  } else if (argc == 3) {
  } else {
    std::cout << "Usage: lox [script]\n";
  }
  return 0;
}

int main(int argc, char *argv[]) {
  doctest::Context context;
  context.addFilter("test-case-exclude", "*math*");
  context.setOption("abort-after", 5);
  context.setOption("order-by", "name");
  context.applyCommandLine(argc, argv);
  context.setOption("no-breaks", true);

  int res = context.run();
  if (context.shouldExit()) {
    return res;
  }

  int client_stuff_return_code = real_main(argc, argv);
  return res + client_stuff_return_code;
}

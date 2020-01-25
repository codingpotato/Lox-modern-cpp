#include <fstream>
#include <iostream>

#include "interpreter.h"
#include "parser.h"
#include "scanner.h"
#include "types.h"

void run(const lox::string &source) {
  try {
    lox::scanner scanner{std::move(source)};
    lox::parser parser{};
    auto program = parser.parse(scanner.scan());
    lox::interpreter interpreter{std::cout};
    interpreter.execute(program);
  } catch (std::exception &e) {
    std::cout << e.what() << '\n';
  }
}

void run_file(const lox::string &filename) {
  std::ifstream ifs(filename);
  run(std::string((std::istreambuf_iterator<char>(ifs)),
                  (std::istreambuf_iterator<char>())));
}

void run_prompt() {}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    run_prompt();
  } else if (argc == 2) {
    run_file(argv[1]);
  } else {
    std::cout << "Usage: lox [script]\n";
  }
  return 1;
}
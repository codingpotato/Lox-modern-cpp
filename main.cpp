#include <iostream>
#include <string>

void run_file(const std::string &) {}

void run_prompt() {}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    std::cout << "Usage: lox [script]\n";
    return 1;
  } else if (argc == 2) {
    run_file(argv[1]);
  } else {
    run_prompt();
  }
}
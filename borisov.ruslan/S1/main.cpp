#include "run.hpp"
#include <iostream>

int main() {
  int result = borisov::run(std::cin, std::cout, std::cerr);
  if (result != 0) return 1;
  return 0;
}

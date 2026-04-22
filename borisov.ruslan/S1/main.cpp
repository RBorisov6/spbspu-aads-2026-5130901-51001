#include "run.hpp"
#include <iostream>

int main() {
  try {
    return borisov::run(std::cin, std::cout, std::cerr);
  } catch (...) {
    std::cerr << "Unexpected error\n";
    return 1;
  }
}

#include "run.hpp"
#include <iostream>

int main() {
  int ret = borisov::run(std::cin, std::cout, std::cerr);
  return (ret != 0) ? 1 : 0;
}

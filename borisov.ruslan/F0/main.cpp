#include "commands.hpp"
#include <iostream>
#include <string>

int main()
{
  borisov::CorpusTable table(16);
  std::string line;
  while (std::getline(std::cin, line))
  {
    if (!line.empty())
    {
      borisov::executeCommand(line, table, std::cout);
    }
  }
  return 0;
}

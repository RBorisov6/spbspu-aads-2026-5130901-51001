#include "io.hpp"
#include "commands.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " <filename>\n";
    return 1;
  }
  std::ifstream file(argv[1]);
  if (!file)
  {
    std::cerr << "Error: cannot open '" << argv[1] << "'\n";
    return 1;
  }
  borisov::DataTable table;
  if (!borisov::readDicts(file, table))
  {
    std::cerr << "Error: failed to parse input\n";
    return 1;
  }
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

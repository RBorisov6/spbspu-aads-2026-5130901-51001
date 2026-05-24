#include "evaluateExpression.hpp"
#include "stack.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

int main(int argc, char* argv[])
{
  std::istream* input = &std::cin;
  std::ifstream fileInput;

  if (argc == 2)
  {
    fileInput.open(argv[1]);
    if (!fileInput.is_open())
    {
      std::cerr << "Error: Cannot open file " << argv[1] << "\n";
      return 1;
    }
    input = &fileInput;
  }

  borisov::Stack< long long > results;
  std::string line;

  while (std::getline(*input, line))
  {
    if (line.empty())
    {
      continue;
    }
    try
    {
      long long res = borisov::evaluateExpression(line, std::cerr);
      results.push(res);
    }
    catch (const std::exception&)
    {
      return 1;
    }
  }

  bool first = true;
  while (!results.empty())
  {
    if (!first)
    {
      std::cout << " ";
    }
    std::cout << results.top();
    results.pop();
    first = false;
  }
  std::cout << "\n";
  return 0;
}

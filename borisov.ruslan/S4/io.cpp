#include "io.hpp"
#include <istream>
#include <sstream>
#include <string>

namespace borisov
{
  bool readDicts(std::istream& in, DataTable& table)
  {
    std::string line;
    while (std::getline(in, line))
    {
      if (line.empty())
      {
        continue;
      }
      std::istringstream ss(line);
      std::string name;
      if (!(ss >> name))
      {
        continue;
      }
      Dict dict;
      int key = 0;
      std::string value;
      while (ss >> key >> value)
      {
        dict.push(key, value);
      }
      if (table.has(name))
      {
        table.get(name) = dict;
      }
      else
      {
        table.push(name, dict);
      }
    }
    return true;
  }
}

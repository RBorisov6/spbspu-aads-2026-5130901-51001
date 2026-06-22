#include "commands.hpp"
#include <sstream>
#include <string>

namespace borisov
{
  namespace
  {
    const char* const INVALID = "<INVALID COMMAND>";
    const char* const EMPTY = "<EMPTY>";
  }

  void cmdPrint(std::istream& in, std::ostream& out, DataTable& table)
  {
    std::string name;
    if (!(in >> name) || !table.has(name))
    {
      out << INVALID << '\n';
      return;
    }
    const Dict& dict = table.get(name);
    if (dict.empty())
    {
      out << EMPTY << '\n';
      return;
    }
    out << name;
    for (auto it = dict.cbegin(); it != dict.cend(); ++it)
    {
      out << ' ' << it->first << ' ' << it->second;
    }
    out << '\n';
  }

  void cmdComplement(std::istream& in, std::ostream& out, DataTable& table)
  {
    std::string newName;
    std::string name1;
    std::string name2;
    if (!(in >> newName >> name1 >> name2))
    {
      out << INVALID << '\n';
      return;
    }
    if (!table.has(name1) || !table.has(name2) || table.has(newName))
    {
      out << INVALID << '\n';
      return;
    }
    const Dict& d1 = table.get(name1);
    const Dict& d2 = table.get(name2);
    Dict result;
    for (auto it = d1.cbegin(); it != d1.cend(); ++it)
    {
      if (!d2.has(it->first))
      {
        result.push(it->first, it->second);
      }
    }
    table.push(newName, result);
  }

  void cmdIntersect(std::istream& in, std::ostream& out, DataTable& table)
  {
    std::string newName;
    std::string name1;
    std::string name2;
    if (!(in >> newName >> name1 >> name2))
    {
      out << INVALID << '\n';
      return;
    }
    if (!table.has(name1) || !table.has(name2) || table.has(newName))
    {
      out << INVALID << '\n';
      return;
    }
    const Dict& d1 = table.get(name1);
    const Dict& d2 = table.get(name2);
    Dict result;
    for (auto it = d1.cbegin(); it != d1.cend(); ++it)
    {
      if (d2.has(it->first))
      {
        result.push(it->first, it->second);
      }
    }
    table.push(newName, result);
  }

  void cmdUnion(std::istream& in, std::ostream& out, DataTable& table)
  {
    std::string newName;
    std::string name1;
    std::string name2;
    if (!(in >> newName >> name1 >> name2))
    {
      out << INVALID << '\n';
      return;
    }
    if (!table.has(name1) || !table.has(name2) || table.has(newName))
    {
      out << INVALID << '\n';
      return;
    }
    const Dict& d1 = table.get(name1);
    const Dict& d2 = table.get(name2);
    Dict result;
    for (auto it = d1.cbegin(); it != d1.cend(); ++it)
    {
      result.push(it->first, it->second);
    }
    for (auto it = d2.cbegin(); it != d2.cend(); ++it)
    {
      if (!d1.has(it->first))
      {
        result.push(it->first, it->second);
      }
    }
    table.push(newName, result);
  }

  void executeCommand(const std::string& line, DataTable& table, std::ostream& out)
  {
    static BSTree< std::string, CmdFunc > cmdTable;
    static bool initialized = false;
    if (!initialized)
    {
      cmdTable.push("print",      cmdPrint);
      cmdTable.push("complement", cmdComplement);
      cmdTable.push("intersect",  cmdIntersect);
      cmdTable.push("union",      cmdUnion);
      initialized = true;
    }
    std::istringstream ss(line);
    std::string cmd;
    if (!(ss >> cmd))
    {
      return;
    }
    if (!cmdTable.has(cmd))
    {
      out << INVALID << '\n';
      return;
    }
    cmdTable.get(cmd)(ss, out, table);
  }
}

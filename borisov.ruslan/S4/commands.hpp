#ifndef BORISOV_COMMANDS_HPP
#define BORISOV_COMMANDS_HPP

#include "io.hpp"
#include <iosfwd>
#include <string>

namespace borisov
{
  using CmdFunc = void(*)(std::istream&, std::ostream&, DataTable&);

  void cmdPrint(std::istream& in, std::ostream& out, DataTable& table);
  void cmdComplement(std::istream& in, std::ostream& out, DataTable& table);
  void cmdIntersect(std::istream& in, std::ostream& out, DataTable& table);
  void cmdUnion(std::istream& in, std::ostream& out, DataTable& table);

  void executeCommand(const std::string& line, DataTable& table, std::ostream& out);
}

#endif

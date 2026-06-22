#ifndef BORISOV_COMMANDS_HPP
#define BORISOV_COMMANDS_HPP

#include "io.hpp"
#include <iosfwd>

namespace borisov
{
  // Execute one line of input as a command.
  // Returns false if command is invalid (prints <INVALID COMMAND>).
  void executeCommand(const std::string& line, GraphTable& graphs,
                      std::ostream& out);

  // Sort a List<string> in-place lexicographically
  void sortStringList(List< std::string >& lst);
}

#endif

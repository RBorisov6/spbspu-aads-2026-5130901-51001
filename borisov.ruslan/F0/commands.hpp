#ifndef BORISOV_COMMANDS_HPP
#define BORISOV_COMMANDS_HPP

#include "corpus.hpp"
#include <iosfwd>
#include <string>

namespace borisov
{
  void executeCommand(const std::string& line, CorpusTable& table, std::ostream& out);
}

#endif

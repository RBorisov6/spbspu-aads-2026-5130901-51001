#ifndef BORISOV_COMMANDS_HPP
#define BORISOV_COMMANDS_HPP

#include "io.hpp"
#include <iosfwd>
#include <string>

namespace borisov
{
  using CmdFunc = void(*)(std::istream&, std::ostream&, GraphTable&);

  void cmdGraphs(std::istream& in, std::ostream& out, GraphTable& graphs);
  void cmdVertexes(std::istream& in, std::ostream& out, GraphTable& graphs);
  void cmdOutbound(std::istream& in, std::ostream& out, GraphTable& graphs);
  void cmdInbound(std::istream& in, std::ostream& out, GraphTable& graphs);
  void cmdBind(std::istream& in, std::ostream& out, GraphTable& graphs);
  void cmdCut(std::istream& in, std::ostream& out, GraphTable& graphs);
  void cmdCreate(std::istream& in, std::ostream& out, GraphTable& graphs);
  void cmdMerge(std::istream& in, std::ostream& out, GraphTable& graphs);
  void cmdExtract(std::istream& in, std::ostream& out, GraphTable& graphs);

  void executeCommand(const std::string& line, GraphTable& graphs, std::ostream& out);
}

#endif

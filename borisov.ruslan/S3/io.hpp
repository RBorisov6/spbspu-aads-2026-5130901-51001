#ifndef BORISOV_IO_HPP
#define BORISOV_IO_HPP

#include "graph.hpp"
#include "hashtable.hpp"
#include "xxhash.hpp"
#include <iosfwd>
#include <string>

namespace borisov
{
  using GraphTable = HashTable< std::string, Graph, XxHash32 >;
  bool readGraphs(std::istream& in, GraphTable& graphs);
}

#endif

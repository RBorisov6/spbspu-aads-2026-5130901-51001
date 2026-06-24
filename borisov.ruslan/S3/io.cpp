#include "io.hpp"
#include <istream>
#include <string>

bool borisov::readGraphs(std::istream& in, borisov::GraphTable& graphs)
{
  std::string graphName;
  while (in >> graphName)
  {
    std::size_t edgeCount = 0;
    if (!(in >> edgeCount))
    {
      return false;
    }
    if (graphs.size() >= graphs.slots())
    {
      graphs.rehash(graphs.slots() * 2 + 1);
    }
    graphs.add(graphName, borisov::Graph());
    borisov::Graph& g = graphs.at(graphName);
    for (std::size_t i = 0; i < edgeCount; ++i)
    {
      std::string from;
      std::string to;
      std::size_t weight = 0;
      if (!(in >> from) || !(in >> to) || !(in >> weight))
      {
        return false;
      }
      g.addEdge(from, to, static_cast< unsigned int >(weight));
    }
  }
  return true;
}

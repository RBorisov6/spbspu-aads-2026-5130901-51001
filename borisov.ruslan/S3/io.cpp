#include "io.hpp"
#include <istream>
#include <string>

namespace borisov
{
  bool readGraphs(std::istream& in, GraphTable& graphs)
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
      graphs.add(graphName, Graph());
      Graph& g = graphs.at(graphName);
      for (std::size_t i = 0; i < edgeCount; ++i)
      {
        std::string from, to;
        std::size_t weight = 0;
        if (!(in >> from) || !(in >> to) || !(in >> weight))
        {
          return false;
        }
        g.addEdge(from, to, static_cast<unsigned int>(weight));
      }
    }
    return true;
  }
}

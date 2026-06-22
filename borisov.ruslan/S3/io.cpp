#include "io.hpp"
#include <iostream>
#include <sstream>
#include <string>

namespace borisov
{
  bool readGraphs(std::istream& in, GraphTable& graphs)
  {
    std::string line;
    while (std::getline(in, line))
    {
      if (line.empty())
      {
        continue;
      }
      std::istringstream header(line);
      std::string graphName;
      std::size_t edgeCount = 0;
      if (!(header >> graphName >> edgeCount))
      {
        return false;
      }

      Graph g;
      for (std::size_t i = 0; i < edgeCount; ++i)
      {
        std::string edgeLine;
        while (std::getline(in, edgeLine))
        {
          if (!edgeLine.empty())
          {
            break;
          }
        }
        std::istringstream edgeStream(edgeLine);
        std::string from, to;
        unsigned int weight = 0;
        if (!(edgeStream >> from >> to >> weight))
        {
          return false;
        }
        g.addEdge(from, to, weight);
      }

      if (graphs.size() >= graphs.slots())
      {
        graphs.rehash(graphs.slots() * 2 + 1);
      }
      graphs.add(graphName, g);
    }
    return true;
  }
}

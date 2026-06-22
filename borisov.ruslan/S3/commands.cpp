#include "commands.hpp"
#include "graph.hpp"
#include "../common/list.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

namespace borisov
{
  namespace
  {
    const char* INVALID = "<INVALID COMMAND>";

    // Collect all keys from a HashTable<string,...> into a List<string>
    List< std::string > collectVertexNames(const Graph::VertexTable& vt)
    {
      List< std::string > names;
      for (auto it = vt.begin(); it != vt.end(); ++it)
      {
        names.pushBack(it->first);
      }
      return names;
    }

    List< std::string > collectGraphNames(const GraphTable& gt)
    {
      List< std::string > names;
      for (auto it = gt.begin(); it != gt.end(); ++it)
      {
        names.pushBack(it->first);
      }
      return names;
    }

    // Sort a List<string> by insertion sort (C++14, no stdlib sort on list)
    void insertionSort(List< std::string >& lst)
    {
      if (lst.size() <= 1)
      {
        return;
      }
      // Build sorted vector then reconstruct list
      std::vector< std::string > v;
      for (auto it = lst.begin(); it != lst.end(); ++it)
      {
        v.push_back(*it);
      }
      std::sort(v.begin(), v.end());
      lst.clear();
      for (auto& s : v)
      {
        lst.pushBack(s);
      }
    }

    void cmdGraphs(GraphTable& graphs, std::ostream& out)
    {
      List< std::string > names = collectGraphNames(graphs);
      insertionSort(names);
      for (auto it = names.begin(); it != names.end(); ++it)
      {
        out << *it << '\n';
      }
    }

    void cmdVertexes(const std::string& graphName, GraphTable& graphs, std::ostream& out)
    {
      if (!graphs.has(graphName))
      {
        out << INVALID << '\n';
        return;
      }
      const Graph& g = graphs.at(graphName);
      List< std::string > names = collectVertexNames(g.vertices());
      insertionSort(names);
      for (auto it = names.begin(); it != names.end(); ++it)
      {
        out << *it << '\n';
      }
    }

    void cmdOutbound(const std::string& graphName, const std::string& vertex,
                     GraphTable& graphs, std::ostream& out)
    {
      if (!graphs.has(graphName))
      {
        out << INVALID << '\n';
        return;
      }
      const Graph& g = graphs.at(graphName);
      if (!g.hasVertex(vertex))
      {
        out << INVALID << '\n';
        return;
      }

      // Collect all destinations from this vertex
      struct EdgeInfo
      {
        std::string to;
        std::vector< unsigned int > weights;
      };
      std::vector< EdgeInfo > edges;

      for (auto it = g.edges().begin(); it != g.edges().end(); ++it)
      {
        if (it->first.first == vertex)
        {
          EdgeInfo e;
          e.to = it->first.second;
          for (auto wt = it->second.begin(); wt != it->second.end(); ++wt)
          {
            e.weights.push_back(*wt);
          }
          std::sort(e.weights.begin(), e.weights.end());
          edges.push_back(e);
        }
      }

      std::sort(edges.begin(), edges.end(),
        [](const EdgeInfo& a, const EdgeInfo& b) { return a.to < b.to; });

      for (auto& e : edges)
      {
        out << e.to;
        for (auto w : e.weights)
        {
          out << ' ' << w;
        }
        out << '\n';
      }
    }

    void cmdInbound(const std::string& graphName, const std::string& vertex,
                    GraphTable& graphs, std::ostream& out)
    {
      if (!graphs.has(graphName))
      {
        out << INVALID << '\n';
        return;
      }
      const Graph& g = graphs.at(graphName);
      if (!g.hasVertex(vertex))
      {
        out << INVALID << '\n';
        return;
      }

      struct EdgeInfo
      {
        std::string from;
        std::vector< unsigned int > weights;
      };
      std::vector< EdgeInfo > edges;

      for (auto it = g.edges().begin(); it != g.edges().end(); ++it)
      {
        if (it->first.second == vertex)
        {
          EdgeInfo e;
          e.from = it->first.first;
          for (auto wt = it->second.begin(); wt != it->second.end(); ++wt)
          {
            e.weights.push_back(*wt);
          }
          std::sort(e.weights.begin(), e.weights.end());
          edges.push_back(e);
        }
      }

      std::sort(edges.begin(), edges.end(),
        [](const EdgeInfo& a, const EdgeInfo& b) { return a.from < b.from; });

      for (auto& e : edges)
      {
        out << e.from;
        for (auto w : e.weights)
        {
          out << ' ' << w;
        }
        out << '\n';
      }
    }

    void cmdBind(const std::string& graphName, const std::string& va,
                 const std::string& vb, unsigned int weight,
                 GraphTable& graphs, std::ostream& out)
    {
      if (!graphs.has(graphName))
      {
        out << INVALID << '\n';
        return;
      }
      graphs.at(graphName).addEdge(va, vb, weight);
    }

    void cmdCut(const std::string& graphName, const std::string& va,
                const std::string& vb, unsigned int weight,
                GraphTable& graphs, std::ostream& out)
    {
      if (!graphs.has(graphName))
      {
        out << INVALID << '\n';
        return;
      }
      Graph& g = graphs.at(graphName);
      if (!g.hasVertex(va) || !g.hasVertex(vb))
      {
        out << INVALID << '\n';
        return;
      }
      try
      {
        g.removeEdge(va, vb, weight);
      }
      catch (const std::out_of_range&)
      {
        out << INVALID << '\n';
      }
    }

    void cmdCreate(const std::string& graphName,
                   const std::vector< std::string >& vertexes,
                   GraphTable& graphs, std::ostream& out)
    {
      if (graphs.has(graphName))
      {
        out << INVALID << '\n';
        return;
      }
      if (graphs.size() >= graphs.slots())
      {
        graphs.rehash(graphs.slots() * 2 + 1);
      }
      Graph g;
      for (auto& v : vertexes)
      {
        g.addVertex(v);
      }
      graphs.add(graphName, g);
    }

    void cmdMerge(const std::string& newName, const std::string& old1,
                  const std::string& old2, GraphTable& graphs, std::ostream& out)
    {
      if (!graphs.has(old1) || !graphs.has(old2))
      {
        out << INVALID << '\n';
        return;
      }
      if (graphs.has(newName))
      {
        out << INVALID << '\n';
        return;
      }
      const Graph& g1 = graphs.at(old1);
      const Graph& g2 = graphs.at(old2);
      Graph merged;

      auto copyEdges = [&](const Graph& src)
      {
        for (auto it = src.edges().begin(); it != src.edges().end(); ++it)
        {
          const std::string& from = it->first.first;
          const std::string& to   = it->first.second;
          for (auto wt = it->second.begin(); wt != it->second.end(); ++wt)
          {
            merged.addEdge(from, to, *wt);
          }
        }
        for (auto it = src.vertices().begin(); it != src.vertices().end(); ++it)
        {
          merged.addVertex(it->first);
        }
      };

      copyEdges(g1);
      copyEdges(g2);

      if (graphs.size() >= graphs.slots())
      {
        graphs.rehash(graphs.slots() * 2 + 1);
      }
      graphs.add(newName, merged);
    }

    void cmdExtract(const std::string& newName, const std::string& oldName,
                    const std::vector< std::string >& vertexes,
                    GraphTable& graphs, std::ostream& out)
    {
      if (!graphs.has(oldName))
      {
        out << INVALID << '\n';
        return;
      }
      if (graphs.has(newName))
      {
        out << INVALID << '\n';
        return;
      }
      const Graph& src = graphs.at(oldName);
      for (auto& v : vertexes)
      {
        if (!src.hasVertex(v))
        {
          out << INVALID << '\n';
          return;
        }
      }

      Graph sub;
      // Build lookup set for selected vertices
      HashTable< std::string, bool, XxHash32 > selected(64);
      for (auto& v : vertexes)
      {
        sub.addVertex(v);
        if (!selected.has(v))
        {
          selected.add(v, true);
        }
      }

      // Copy edges where both endpoints are in selected set
      for (auto it = src.edges().begin(); it != src.edges().end(); ++it)
      {
        const std::string& from = it->first.first;
        const std::string& to   = it->first.second;
        if (selected.has(from) && selected.has(to))
        {
          for (auto wt = it->second.begin(); wt != it->second.end(); ++wt)
          {
            sub.addEdge(from, to, *wt);
          }
        }
      }

      if (graphs.size() >= graphs.slots())
      {
        graphs.rehash(graphs.slots() * 2 + 1);
      }
      graphs.add(newName, sub);
    }
  }

  void sortStringList(List< std::string >& lst)
  {
    std::vector< std::string > v;
    for (auto it = lst.begin(); it != lst.end(); ++it)
    {
      v.push_back(*it);
    }
    std::sort(v.begin(), v.end());
    lst.clear();
    for (auto& s : v)
    {
      lst.pushBack(s);
    }
  }

  void executeCommand(const std::string& line, GraphTable& graphs, std::ostream& out)
  {
    std::istringstream ss(line);
    std::string cmd;
    if (!(ss >> cmd))
    {
      return;
    }

    if (cmd == "graphs")
    {
      cmdGraphs(graphs, out);
    }
    else if (cmd == "vertexes")
    {
      std::string graphName;
      if (!(ss >> graphName))
      {
        out << INVALID << '\n';
        return;
      }
      cmdVertexes(graphName, graphs, out);
    }
    else if (cmd == "outbound")
    {
      std::string graphName, vertex;
      if (!(ss >> graphName >> vertex))
      {
        out << INVALID << '\n';
        return;
      }
      cmdOutbound(graphName, vertex, graphs, out);
    }
    else if (cmd == "inbound")
    {
      std::string graphName, vertex;
      if (!(ss >> graphName >> vertex))
      {
        out << INVALID << '\n';
        return;
      }
      cmdInbound(graphName, vertex, graphs, out);
    }
    else if (cmd == "bind")
    {
      std::string graphName, va, vb;
      unsigned int weight = 0;
      if (!(ss >> graphName >> va >> vb >> weight))
      {
        out << INVALID << '\n';
        return;
      }
      cmdBind(graphName, va, vb, weight, graphs, out);
    }
    else if (cmd == "cut")
    {
      std::string graphName, va, vb;
      unsigned int weight = 0;
      if (!(ss >> graphName >> va >> vb >> weight))
      {
        out << INVALID << '\n';
        return;
      }
      cmdCut(graphName, va, vb, weight, graphs, out);
    }
    else if (cmd == "create")
    {
      std::string graphName;
      if (!(ss >> graphName))
      {
        out << INVALID << '\n';
        return;
      }
      std::size_t k = 0;
      ss >> k;  // optional; if absent k stays 0
      std::vector< std::string > vertexes;
      for (std::size_t i = 0; i < k; ++i)
      {
        std::string v;
        if (!(ss >> v))
        {
          out << INVALID << '\n';
          return;
        }
        vertexes.push_back(v);
      }
      cmdCreate(graphName, vertexes, graphs, out);
    }
    else if (cmd == "merge")
    {
      std::string newName, old1, old2;
      if (!(ss >> newName >> old1 >> old2))
      {
        out << INVALID << '\n';
        return;
      }
      cmdMerge(newName, old1, old2, graphs, out);
    }
    else if (cmd == "extract")
    {
      std::string newName, oldName;
      std::size_t k = 0;
      if (!(ss >> newName >> oldName >> k))
      {
        out << INVALID << '\n';
        return;
      }
      std::vector< std::string > vertexes;
      for (std::size_t i = 0; i < k; ++i)
      {
        std::string v;
        if (!(ss >> v))
        {
          out << INVALID << '\n';
          return;
        }
        vertexes.push_back(v);
      }
      cmdExtract(newName, oldName, vertexes, graphs, out);
    }
    else
    {
      out << INVALID << '\n';
    }
  }
}

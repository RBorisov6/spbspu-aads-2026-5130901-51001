#include "commands.hpp"
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

namespace borisov
{
  namespace
  {
    const char* INVALID = "<INVALID COMMAND>";

    void ensureGraphSlot(GraphTable& graphs)
    {
      if (graphs.size() >= graphs.slots())
      {
        graphs.rehash(graphs.slots() * 2 + 1);
      }
    }

    void printEdges(std::ostream& out,
      const List< std::pair< std::string, unsigned int > >& edges)
    {
      std::vector< std::pair< std::string, unsigned int > > v;
      for (auto it = edges.begin(); it != edges.end(); ++it)
      {
        v.push_back(*it);
      }
      std::sort(v.begin(), v.end(),
        [](const std::pair< std::string, unsigned int >& a,
           const std::pair< std::string, unsigned int >& b)
        {
          return a.first < b.first || (a.first == b.first && a.second < b.second);
        });
      std::size_t i = 0;
      while (i < v.size())
      {
        std::string name = v[i].first;
        out << name;
        while (i < v.size() && v[i].first == name)
        {
          out << ' ' << v[i].second;
          ++i;
        }
        out << '\n';
      }
    }
  }

  void cmdGraphs(std::istream&, std::ostream& out, GraphTable& graphs)
  {
    std::vector< std::string > names;
    for (auto it = graphs.begin(); it != graphs.end(); ++it)
    {
      names.push_back(it->first);
    }
    std::sort(names.begin(), names.end());
    for (auto& n : names)
    {
      out << n << '\n';
    }
  }

  void cmdVertexes(std::istream& in, std::ostream& out, GraphTable& graphs)
  {
    std::string graphName;
    if (!(in >> graphName) || !graphs.has(graphName))
    {
      out << INVALID << '\n';
      return;
    }
    auto verts = graphs.at(graphName).getVertexes();
    std::vector< std::string > v;
    for (auto it = verts.begin(); it != verts.end(); ++it)
    {
      v.push_back(*it);
    }
    std::sort(v.begin(), v.end());
    for (auto& s : v)
    {
      out << s << '\n';
    }
  }

  void cmdOutbound(std::istream& in, std::ostream& out, GraphTable& graphs)
  {
    std::string graphName, vertex;
    if (!(in >> graphName >> vertex) || !graphs.has(graphName))
    {
      out << INVALID << '\n';
      return;
    }
    try
    {
      printEdges(out, graphs.at(graphName).getOutbound(vertex));
    }
    catch (const std::out_of_range&)
    {
      out << INVALID << '\n';
    }
  }

  void cmdInbound(std::istream& in, std::ostream& out, GraphTable& graphs)
  {
    std::string graphName, vertex;
    if (!(in >> graphName >> vertex) || !graphs.has(graphName))
    {
      out << INVALID << '\n';
      return;
    }
    try
    {
      printEdges(out, graphs.at(graphName).getInbound(vertex));
    }
    catch (const std::out_of_range&)
    {
      out << INVALID << '\n';
    }
  }

  void cmdBind(std::istream& in, std::ostream& out, GraphTable& graphs)
  {
    std::string graphName, va, vb;
    unsigned int weight = 0;
    if (!(in >> graphName >> va >> vb >> weight) || !graphs.has(graphName))
    {
      out << INVALID << '\n';
      return;
    }
    graphs.at(graphName).addEdge(va, vb, weight);
  }

  void cmdCut(std::istream& in, std::ostream& out, GraphTable& graphs)
  {
    std::string graphName, va, vb;
    unsigned int weight = 0;
    if (!(in >> graphName >> va >> vb >> weight) || !graphs.has(graphName))
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

  void cmdCreate(std::istream& in, std::ostream& out, GraphTable& graphs)
  {
    std::string graphName;
    if (!(in >> graphName))
    {
      out << INVALID << '\n';
      return;
    }
    if (graphs.has(graphName))
    {
      out << INVALID << '\n';
      return;
    }
    std::size_t k = 0;
    in >> k;
    std::vector< std::string > vertexes;
    for (std::size_t i = 0; i < k; ++i)
    {
      std::string v;
      if (!(in >> v))
      {
        out << INVALID << '\n';
        return;
      }
      vertexes.push_back(v);
    }
    ensureGraphSlot(graphs);
    graphs.add(graphName, Graph());
    Graph& g = graphs.at(graphName);
    for (auto& v : vertexes)
    {
      g.addVertex(v);
    }
  }

  void cmdMerge(std::istream& in, std::ostream& out, GraphTable& graphs)
  {
    std::string newName, old1, old2;
    if (!(in >> newName >> old1 >> old2))
    {
      out << INVALID << '\n';
      return;
    }
    if (!graphs.has(old1) || !graphs.has(old2) || graphs.has(newName))
    {
      out << INVALID << '\n';
      return;
    }
    Graph merged;
    auto copyEdges = [&](const Graph& src)
    {
      for (auto it = src.outgoing().begin(); it != src.outgoing().end(); ++it)
      {
        const std::string& from = it->first;
        merged.addVertex(from);
        const AdjTable& adj = it->second;
        for (auto jt = adj.begin(); jt != adj.end(); ++jt)
        {
          for (auto wt = jt->second.begin(); wt != jt->second.end(); ++wt)
          {
            merged.addEdge(from, jt->first, *wt);
          }
        }
      }
    };
    copyEdges(graphs.at(old1));
    copyEdges(graphs.at(old2));
    ensureGraphSlot(graphs);
    graphs.add(newName, merged);
  }

  void cmdExtract(std::istream& in, std::ostream& out, GraphTable& graphs)
  {
    std::string newName, oldName;
    std::size_t k = 0;
    if (!(in >> newName >> oldName >> k))
    {
      out << INVALID << '\n';
      return;
    }
    if (!graphs.has(oldName) || graphs.has(newName))
    {
      out << INVALID << '\n';
      return;
    }
    std::vector< std::string > vertexes;
    for (std::size_t i = 0; i < k; ++i)
    {
      std::string v;
      if (!(in >> v))
      {
        out << INVALID << '\n';
        return;
      }
      vertexes.push_back(v);
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
    HashTable< std::string, bool, XxHash32 > selected(64);
    for (auto& v : vertexes)
    {
      if (!selected.has(v))
      {
        if (selected.size() >= selected.slots())
        {
          selected.rehash(selected.slots() * 2 + 1);
        }
        selected.add(v, true);
      }
    }
    Graph sub;
    for (auto& v : vertexes)
    {
      sub.addVertex(v);
    }
    for (auto it = src.outgoing().begin(); it != src.outgoing().end(); ++it)
    {
      const std::string& from = it->first;
      if (!selected.has(from))
      {
        continue;
      }
      const AdjTable& adj = it->second;
      for (auto jt = adj.begin(); jt != adj.end(); ++jt)
      {
        if (!selected.has(jt->first))
        {
          continue;
        }
        for (auto wt = jt->second.begin(); wt != jt->second.end(); ++wt)
        {
          sub.addEdge(from, jt->first, *wt);
        }
      }
    }
    ensureGraphSlot(graphs);
    graphs.add(newName, sub);
  }

  void executeCommand(const std::string& line, GraphTable& graphs, std::ostream& out)
  {
    static HashTable< std::string, CmdFunc, XxHash32 > cmdTable(16);
    static bool initialized = false;
    if (!initialized)
    {
      cmdTable.add("graphs",   cmdGraphs);
      cmdTable.add("vertexes", cmdVertexes);
      cmdTable.add("outbound", cmdOutbound);
      cmdTable.add("inbound",  cmdInbound);
      cmdTable.add("bind",     cmdBind);
      cmdTable.add("cut",      cmdCut);
      cmdTable.add("create",   cmdCreate);
      cmdTable.add("merge",    cmdMerge);
      cmdTable.add("extract",  cmdExtract);
      initialized = true;
    }
    std::istringstream ss(line);
    std::string cmd;
    if (!(ss >> cmd))
    {
      return;
    }
    try
    {
      cmdTable.at(cmd)(ss, out, graphs);
      if (ss.fail())
      {
        out << INVALID << '\n';
      }
    }
    catch (const std::out_of_range&)
    {
      out << INVALID << '\n';
    }
  }
}

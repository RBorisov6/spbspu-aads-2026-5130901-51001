#include "commands.hpp"
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

namespace
{
  const char* const INVALID = "<INVALID COMMAND>";

  void ensureGraphSlot(borisov::GraphTable& graphs)
  {
    if (graphs.size() >= graphs.slots())
    {
      graphs.rehash(graphs.slots() * 2 + 1);
    }
  }

  void printEdges(std::ostream& out,
    const borisov::List< std::pair< std::string, unsigned int > >& edges)
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
    if (v.empty())
    {
      out << '\n';
      return;
    }
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

void borisov::cmdGraphs(std::istream&, std::ostream& out, borisov::GraphTable& graphs)
{
  std::vector< std::string > names;
  for (auto it = graphs.begin(); it != graphs.end(); ++it)
  {
    names.push_back(it->first);
  }
  std::sort(names.begin(), names.end());
  if (names.empty())
  {
    out << '\n';
    return;
  }
  for (const std::string& n : names)
  {
    out << n << '\n';
  }
}

void borisov::cmdVertexes(std::istream& in, std::ostream& out, borisov::GraphTable& graphs)
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
  if (v.empty())
  {
    out << '\n';
    return;
  }
  for (const std::string& s : v)
  {
    out << s << '\n';
  }
}

void borisov::cmdOutbound(std::istream& in, std::ostream& out, borisov::GraphTable& graphs)
{
  std::string graphName;
  std::string vertex;
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

void borisov::cmdInbound(std::istream& in, std::ostream& out, borisov::GraphTable& graphs)
{
  std::string graphName;
  std::string vertex;
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

void borisov::cmdBind(std::istream& in, std::ostream& out, borisov::GraphTable& graphs)
{
  std::string graphName;
  std::string va;
  std::string vb;
  unsigned int weight = 0;
  if (!(in >> graphName >> va >> vb >> weight) || !graphs.has(graphName))
  {
    out << INVALID << '\n';
    return;
  }
  graphs.at(graphName).addEdge(va, vb, weight);
}

void borisov::cmdCut(std::istream& in, std::ostream& out, borisov::GraphTable& graphs)
{
  std::string graphName;
  std::string va;
  std::string vb;
  unsigned int weight = 0;
  if (!(in >> graphName >> va >> vb >> weight) || !graphs.has(graphName))
  {
    out << INVALID << '\n';
    return;
  }
  borisov::Graph& g = graphs.at(graphName);
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

void borisov::cmdCreate(std::istream& in, std::ostream& out, borisov::GraphTable& graphs)
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
  graphs.add(graphName, borisov::Graph());
  borisov::Graph& g = graphs.at(graphName);
  for (const std::string& v : vertexes)
  {
    g.addVertex(v);
  }
}

void borisov::cmdMerge(std::istream& in, std::ostream& out, borisov::GraphTable& graphs)
{
  std::string newName;
  std::string old1;
  std::string old2;
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
  borisov::Graph merged;
  auto copyEdges = [&](const borisov::Graph& src)
  {
    for (auto it = src.outgoing().begin(); it != src.outgoing().end(); ++it)
    {
      const std::string& from = it->first;
      merged.addVertex(from);
      const borisov::AdjTable& adj = it->second;
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

void borisov::cmdExtract(std::istream& in, std::ostream& out, borisov::GraphTable& graphs)
{
  std::string newName;
  std::string oldName;
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
  const borisov::Graph& src = graphs.at(oldName);
  for (const std::string& v : vertexes)
  {
    if (!src.hasVertex(v))
    {
      out << INVALID << '\n';
      return;
    }
  }
  borisov::HashTable< std::string, bool, borisov::XxHash32 > selected(64);
  for (const std::string& v : vertexes)
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
  borisov::Graph sub;
  for (const std::string& v : vertexes)
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
    const borisov::AdjTable& adj = it->second;
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

void borisov::executeCommand(const std::string& line,
  borisov::GraphTable& graphs, std::ostream& out)
{
  static borisov::HashTable< std::string, borisov::CmdFunc, borisov::XxHash32 > cmdTable(16);
  static bool initialized = false;
  if (!initialized)
  {
    cmdTable.add("graphs",   borisov::cmdGraphs);
    cmdTable.add("vertexes", borisov::cmdVertexes);
    cmdTable.add("outbound", borisov::cmdOutbound);
    cmdTable.add("inbound",  borisov::cmdInbound);
    cmdTable.add("bind",     borisov::cmdBind);
    cmdTable.add("cut",      borisov::cmdCut);
    cmdTable.add("create",   borisov::cmdCreate);
    cmdTable.add("merge",    borisov::cmdMerge);
    cmdTable.add("extract",  borisov::cmdExtract);
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
  }
  catch (const std::out_of_range&)
  {
    out << INVALID << '\n';
  }
}

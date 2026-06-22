#include "graph.hpp"

static const std::size_t INIT = 64;
static const std::size_t SEEN_INIT = 128;

borisov::Graph::Graph():
  outgoing_(INIT),
  incoming_(INIT)
{}

const borisov::AdjMap& borisov::Graph::outgoing() const
{
  return outgoing_;
}

borisov::AdjMap& borisov::Graph::outgoing()
{
  return outgoing_;
}

const borisov::AdjMap& borisov::Graph::incoming() const
{
  return incoming_;
}

borisov::AdjMap& borisov::Graph::incoming()
{
  return incoming_;
}

void borisov::Graph::ensureIn(borisov::AdjMap& table, const std::string& v)
{
  if (!table.has(v))
  {
    if (table.size() >= table.slots())
    {
      table.rehash(table.slots() * 2 + 1);
    }
    table.add(v, AdjTable(INIT));
  }
}

void borisov::Graph::addWeightToAdj(borisov::AdjTable& adj,
  const std::string& neighbor, unsigned int w)
{
  if (!adj.has(neighbor))
  {
    if (adj.size() >= adj.slots())
    {
      adj.rehash(adj.slots() * 2 + 1);
    }
    adj.add(neighbor, List< unsigned int >());
  }
  adj.at(neighbor).pushBack(w);
}

void borisov::Graph::removeWeightFromAdj(borisov::AdjTable& adj,
  const std::string& neighbor, unsigned int w)
{
  List< unsigned int >& weights = adj.at(neighbor);
  for (auto it = weights.begin(); it != weights.end(); ++it)
  {
    if (*it == w)
    {
      weights.erase(it);
      if (weights.empty())
      {
        adj.drop(neighbor);
      }
      return;
    }
  }
  throw std::out_of_range("weight not found");
}

void borisov::Graph::addVertex(const std::string& v)
{
  ensureIn(outgoing_, v);
}

bool borisov::Graph::hasVertex(const std::string& v) const
{
  return outgoing_.has(v) || incoming_.has(v);
}

void borisov::Graph::addEdge(const std::string& from, const std::string& to, unsigned int w)
{
  ensureIn(outgoing_, from);
  ensureIn(outgoing_, to);
  ensureIn(incoming_, from);
  ensureIn(incoming_, to);
  addWeightToAdj(outgoing_.at(from), to, w);
  addWeightToAdj(incoming_.at(to), from, w);
}

void borisov::Graph::removeEdge(const std::string& from, const std::string& to, unsigned int w)
{
  if (!outgoing_.has(from) || !outgoing_.at(from).has(to))
  {
    throw std::out_of_range("edge not found");
  }
  removeWeightFromAdj(outgoing_.at(from), to, w);
  removeWeightFromAdj(incoming_.at(to), from, w);
}

borisov::List< std::string > borisov::Graph::getVertexes() const
{
  HashTable< std::string, bool, XxHash32 > seen(SEEN_INIT);
  List< std::string > result;

  auto addIfNew = [&](const std::string& name)
  {
    if (!seen.has(name))
    {
      if (seen.size() >= seen.slots())
      {
        seen.rehash(seen.slots() * 2 + 1);
      }
      seen.add(name, true);
      result.pushBack(name);
    }
  };

  for (auto it = outgoing_.begin(); it != outgoing_.end(); ++it)
  {
    addIfNew(it->first);
  }
  for (auto it = incoming_.begin(); it != incoming_.end(); ++it)
  {
    addIfNew(it->first);
  }
  return result;
}

borisov::List< std::pair< std::string, unsigned int > >
borisov::Graph::getOutbound(const std::string& v) const
{
  if (!hasVertex(v))
  {
    throw std::out_of_range("vertex not found");
  }
  List< std::pair< std::string, unsigned int > > result;
  if (!outgoing_.has(v))
  {
    return result;
  }
  const AdjTable& adj = outgoing_.at(v);
  for (auto it = adj.begin(); it != adj.end(); ++it)
  {
    for (auto wt = it->second.begin(); wt != it->second.end(); ++wt)
    {
      result.pushBack(std::make_pair(it->first, *wt));
    }
  }
  return result;
}

borisov::List< std::pair< std::string, unsigned int > >
borisov::Graph::getInbound(const std::string& v) const
{
  if (!hasVertex(v))
  {
    throw std::out_of_range("vertex not found");
  }
  List< std::pair< std::string, unsigned int > > result;
  if (!incoming_.has(v))
  {
    return result;
  }
  const AdjTable& adj = incoming_.at(v);
  for (auto it = adj.begin(); it != adj.end(); ++it)
  {
    for (auto wt = it->second.begin(); wt != it->second.end(); ++wt)
    {
      result.pushBack(std::make_pair(it->first, *wt));
    }
  }
  return result;
}

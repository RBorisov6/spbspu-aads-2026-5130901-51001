#ifndef BORISOV_GRAPH_HPP
#define BORISOV_GRAPH_HPP

#include "hashtable.hpp"
#include "xxhash.hpp"
#include "../common/list.hpp"
#include <string>
#include <utility>
#include <stdexcept>

namespace borisov
{
  using AdjTable = HashTable< std::string, List< unsigned int >, XxHash32 >;
  using AdjMap   = HashTable< std::string, AdjTable, XxHash32 >;

  class Graph
  {
  public:
    Graph();

    void addVertex(const std::string& v);
    bool hasVertex(const std::string& v) const;
    void addEdge(const std::string& from, const std::string& to, unsigned int w);
    void removeEdge(const std::string& from, const std::string& to, unsigned int w);

    List< std::string > getVertexes() const;
    List< std::pair< std::string, unsigned int > > getOutbound(const std::string& v) const;
    List< std::pair< std::string, unsigned int > > getInbound(const std::string& v) const;

    const AdjMap& outgoing() const;
    const AdjMap& incoming() const;
    AdjMap& outgoing();
    AdjMap& incoming();

  private:
    AdjMap outgoing_;
    AdjMap incoming_;

    static void ensureIn(AdjMap& table, const std::string& v);
    static void addWeightToAdj(AdjTable& adj, const std::string& neighbor, unsigned int w);
    static void removeWeightFromAdj(AdjTable& adj, const std::string& neighbor, unsigned int w);
  };
}

#endif

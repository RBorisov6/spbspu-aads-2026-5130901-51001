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
  // Directed weighted graph with multiple edges allowed between same vertex pair.
  // Vertices and edges are stored in HashTables keyed by xxHash32.
  class Graph
  {
  public:
    using EdgeKey = std::pair< std::string, std::string >;
    using Weights = List< unsigned int >;
    using VertexTable = HashTable< std::string, bool, XxHash32 >;
    using EdgeTable   = HashTable< EdgeKey, Weights, XxHash32 >;

    static const std::size_t INIT_SLOTS = 64;

    Graph():
      vertices_(INIT_SLOTS),
      edges_(INIT_SLOTS)
    {}

    void addVertex(const std::string& name)
    {
      if (!vertices_.has(name))
      {
        ensureVertexSlot();
        vertices_.add(name, true);
      }
    }

    bool hasVertex(const std::string& name) const
    {
      return vertices_.has(name);
    }

    // Add a directed edge from->to with given weight.
    // Automatically registers both vertices.
    void addEdge(const std::string& from, const std::string& to, unsigned int weight)
    {
      addVertex(from);
      addVertex(to);
      EdgeKey k(from, to);
      if (!edges_.has(k))
      {
        ensureEdgeSlot();
        Weights w;
        w.pushBack(weight);
        edges_.add(k, w);
      }
      else
      {
        edges_.at(k).pushBack(weight);
      }
    }

    // Remove one occurrence of the edge from->to with given weight.
    // Throws std::out_of_range if the edge doesn't exist.
    void removeEdge(const std::string& from, const std::string& to, unsigned int weight)
    {
      EdgeKey k(from, to);
      if (!edges_.has(k))
      {
        throw std::out_of_range("Graph::removeEdge: edge not found");
      }
      Weights& ws = edges_.at(k);
      for (auto it = ws.begin(); it != ws.end(); ++it)
      {
        if (*it == weight)
        {
          ws.erase(it);
          if (ws.empty())
          {
            edges_.drop(k);
          }
          return;
        }
      }
      throw std::out_of_range("Graph::removeEdge: weight not found");
    }

    const VertexTable& vertices() const { return vertices_; }
    const EdgeTable& edges() const { return edges_; }

    VertexTable& vertices() { return vertices_; }
    EdgeTable& edges() { return edges_; }

  private:
    VertexTable vertices_;
    EdgeTable   edges_;

    void ensureVertexSlot()
    {
      if (vertices_.size() >= vertices_.slots())
      {
        vertices_.rehash(vertices_.slots() * 2 + 1);
      }
    }

    void ensureEdgeSlot()
    {
      if (edges_.size() >= edges_.slots())
      {
        edges_.rehash(edges_.slots() * 2 + 1);
      }
    }
  };
}

#endif

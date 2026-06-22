#include <boost/test/unit_test.hpp>
#include "graph.hpp"
#include <stdexcept>

BOOST_AUTO_TEST_CASE(graph_add_vertex)
{
  borisov::Graph g;
  g.addVertex("a");
  BOOST_CHECK(g.hasVertex("a"));
  BOOST_CHECK(!g.hasVertex("b"));
}

BOOST_AUTO_TEST_CASE(graph_add_vertex_twice_is_ok)
{
  borisov::Graph g;
  g.addVertex("a");
  g.addVertex("a");
  BOOST_CHECK(g.hasVertex("a"));
}

BOOST_AUTO_TEST_CASE(graph_add_edge_registers_vertices)
{
  borisov::Graph g;
  g.addEdge("x", "y", 5);
  BOOST_CHECK(g.hasVertex("x"));
  BOOST_CHECK(g.hasVertex("y"));
}

BOOST_AUTO_TEST_CASE(graph_add_edge_stored_in_edges)
{
  borisov::Graph g;
  g.addEdge("a", "b", 10);
  auto key = std::make_pair(std::string("a"), std::string("b"));
  BOOST_CHECK(g.edges().has(key));
  const auto& ws = g.edges().at(key);
  BOOST_CHECK_EQUAL(ws.front(), 10u);
}

BOOST_AUTO_TEST_CASE(graph_multiple_weights_same_edge)
{
  borisov::Graph g;
  g.addEdge("a", "b", 1);
  g.addEdge("a", "b", 2);
  auto key = std::make_pair(std::string("a"), std::string("b"));
  const auto& ws = g.edges().at(key);
  BOOST_CHECK_EQUAL(ws.size(), 2u);
}

BOOST_AUTO_TEST_CASE(graph_remove_edge_ok)
{
  borisov::Graph g;
  g.addEdge("a", "b", 5);
  g.removeEdge("a", "b", 5);
  auto key = std::make_pair(std::string("a"), std::string("b"));
  BOOST_CHECK(!g.edges().has(key));
}

BOOST_AUTO_TEST_CASE(graph_remove_one_weight_from_multi)
{
  borisov::Graph g;
  g.addEdge("a", "b", 3);
  g.addEdge("a", "b", 7);
  g.removeEdge("a", "b", 3);
  auto key = std::make_pair(std::string("a"), std::string("b"));
  const auto& ws = g.edges().at(key);
  BOOST_CHECK_EQUAL(ws.size(), 1u);
  BOOST_CHECK_EQUAL(ws.front(), 7u);
}

BOOST_AUTO_TEST_CASE(graph_remove_nonexistent_edge_throws)
{
  borisov::Graph g;
  BOOST_CHECK_THROW(g.removeEdge("a", "b", 1), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(graph_remove_wrong_weight_throws)
{
  borisov::Graph g;
  g.addEdge("a", "b", 5);
  BOOST_CHECK_THROW(g.removeEdge("a", "b", 99), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(graph_self_loop)
{
  borisov::Graph g;
  g.addEdge("a", "a", 0);
  auto key = std::make_pair(std::string("a"), std::string("a"));
  BOOST_CHECK(g.edges().has(key));
}

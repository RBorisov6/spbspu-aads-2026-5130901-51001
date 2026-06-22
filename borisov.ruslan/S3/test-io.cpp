#include <boost/test/unit_test.hpp>
#include "io.hpp"
#include <sstream>

BOOST_AUTO_TEST_CASE(io_read_single_graph)
{
  std::istringstream in(
    "gr1 2\n"
    "a b 10\n"
    "b c 20\n"
  );
  borisov::GraphTable graphs(16);
  BOOST_CHECK(borisov::readGraphs(in, graphs));
  BOOST_CHECK(graphs.has("gr1"));
  auto& g = graphs.at("gr1");
  BOOST_CHECK(g.hasVertex("a"));
  BOOST_CHECK(g.hasVertex("b"));
  BOOST_CHECK(g.hasVertex("c"));
  BOOST_CHECK(!g.getOutbound("a").empty());
}

BOOST_AUTO_TEST_CASE(io_read_multiple_graphs)
{
  std::istringstream in(
    "gr1 1\n"
    "a b 5\n"
    "gr2 1\n"
    "x y 7\n"
  );
  borisov::GraphTable graphs(16);
  BOOST_CHECK(borisov::readGraphs(in, graphs));
  BOOST_CHECK(graphs.has("gr1"));
  BOOST_CHECK(graphs.has("gr2"));
}

BOOST_AUTO_TEST_CASE(io_blank_lines_ignored)
{
  std::istringstream in(
    "\n"
    "gr1 1\n"
    "\n"
    "a b 3\n"
    "\n"
  );
  borisov::GraphTable graphs(16);
  BOOST_CHECK(borisov::readGraphs(in, graphs));
  BOOST_CHECK(graphs.has("gr1"));
}

BOOST_AUTO_TEST_CASE(io_zero_edges_graph)
{
  std::istringstream in("empty 0\n");
  borisov::GraphTable graphs(16);
  BOOST_CHECK(borisov::readGraphs(in, graphs));
  BOOST_CHECK(graphs.has("empty"));
}

BOOST_AUTO_TEST_CASE(io_self_loop_edge)
{
  std::istringstream in("g 1\nb b 2\n");
  borisov::GraphTable graphs(16);
  BOOST_CHECK(borisov::readGraphs(in, graphs));
  auto& g = graphs.at("g");
  BOOST_CHECK(!g.getOutbound("b").empty());
}

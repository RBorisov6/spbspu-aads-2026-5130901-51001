#include <boost/test/unit_test.hpp>
#include "commands.hpp"
#include "io.hpp"
#include <sstream>
#include <string>

namespace
{
  // Helper: build a GraphTable from a string describing the input file
  borisov::GraphTable makeGraphs(const std::string& input)
  {
    borisov::GraphTable graphs(16);
    std::istringstream in(input);
    borisov::readGraphs(in, graphs);
    return graphs;
  }

  std::string run(borisov::GraphTable& graphs, const std::string& cmd)
  {
    std::ostringstream out;
    borisov::executeCommand(cmd, graphs, out);
    return out.str();
  }
}

BOOST_AUTO_TEST_CASE(cmd_graphs_sorted)
{
  borisov::GraphTable graphs = makeGraphs(
    "gr2 1\na b 1\n"
    "gr1 1\nc d 2\n"
  );
  std::string result = run(graphs, "graphs");
  BOOST_CHECK_EQUAL(result, "gr1\ngr2\n");
}

BOOST_AUTO_TEST_CASE(cmd_vertexes_sorted)
{
  borisov::GraphTable graphs = makeGraphs(
    "gr1 3\nc a 1\na b 2\nb c 3\n"
  );
  std::string result = run(graphs, "vertexes gr1");
  BOOST_CHECK_EQUAL(result, "a\nb\nc\n");
}

BOOST_AUTO_TEST_CASE(cmd_vertexes_invalid_graph)
{
  borisov::GraphTable graphs = makeGraphs("gr1 0\n");
  std::string result = run(graphs, "vertexes gr99");
  BOOST_CHECK_EQUAL(result, "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_outbound_sorted_by_dest_and_weights)
{
  borisov::GraphTable graphs = makeGraphs(
    "gr1 4\nc a 30\nc b 20\na b 40\nb c 50\n"
  );
  std::string result = run(graphs, "outbound gr1 c");
  BOOST_CHECK_EQUAL(result, "a 30\nb 20\n");
}

BOOST_AUTO_TEST_CASE(cmd_outbound_invalid_graph)
{
  borisov::GraphTable graphs = makeGraphs("gr1 0\n");
  BOOST_CHECK_EQUAL(run(graphs, "outbound gr99 x"), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_outbound_invalid_vertex)
{
  borisov::GraphTable graphs = makeGraphs("gr1 1\na b 1\n");
  BOOST_CHECK_EQUAL(run(graphs, "outbound gr1 z"), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_inbound_sorted_by_source_and_weights)
{
  borisov::GraphTable graphs = makeGraphs(
    "gr1 4\na b 40\nc b 20\na c 30\nb c 50\n"
  );
  std::string result = run(graphs, "inbound gr1 b");
  BOOST_CHECK_EQUAL(result, "a 40\nc 20\n");
}

BOOST_AUTO_TEST_CASE(cmd_inbound_invalid_graph)
{
  borisov::GraphTable graphs = makeGraphs("gr1 0\n");
  BOOST_CHECK_EQUAL(run(graphs, "inbound gr99 x"), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_inbound_invalid_vertex)
{
  borisov::GraphTable graphs = makeGraphs("gr1 1\na b 1\n");
  BOOST_CHECK_EQUAL(run(graphs, "inbound gr1 z"), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_unknown_command_is_invalid)
{
  borisov::GraphTable graphs(4);
  BOOST_CHECK_EQUAL(run(graphs, "foobar"), "<INVALID COMMAND>\n");
}

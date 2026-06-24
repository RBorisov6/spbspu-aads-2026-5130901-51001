#include <boost/test/unit_test.hpp>
#include "commands.hpp"
#include "io.hpp"
#include <sstream>
#include <string>

namespace
{
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

BOOST_AUTO_TEST_CASE(cmd_bind_adds_edge)
{
  borisov::GraphTable graphs = makeGraphs("gr2 1\na b 1\n");
  run(graphs, "bind gr2 b c 100");
  std::string result = run(graphs, "outbound gr2 b");
  BOOST_CHECK_EQUAL(result, "c 100\n");
}

BOOST_AUTO_TEST_CASE(cmd_bind_creates_new_vertex)
{
  borisov::GraphTable graphs = makeGraphs("gr2 1\na b 1\n");
  run(graphs, "bind gr2 b d 200");
  std::string verts = run(graphs, "vertexes gr2");
  BOOST_CHECK(verts.find('d') != std::string::npos);
}

BOOST_AUTO_TEST_CASE(cmd_bind_invalid_graph)
{
  borisov::GraphTable graphs(4);
  BOOST_CHECK_EQUAL(run(graphs, "bind noGraph a b 1"), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_cut_removes_edge)
{
  borisov::GraphTable graphs = makeGraphs("gr2 2\nb b 2\na b 1\n");
  run(graphs, "cut gr2 b b 2");
  std::string result = run(graphs, "inbound gr2 b");
  BOOST_CHECK(result.find("b 2") == std::string::npos);
  BOOST_CHECK(result.find("a 1") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(cmd_cut_invalid_graph)
{
  borisov::GraphTable graphs(4);
  BOOST_CHECK_EQUAL(run(graphs, "cut noGraph a b 1"), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_cut_invalid_weight)
{
  borisov::GraphTable graphs = makeGraphs("gr2 1\nb b 2\n");
  BOOST_CHECK_EQUAL(run(graphs, "cut gr2 b b 1"), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_create_new_graph)
{
  borisov::GraphTable graphs(16);
  run(graphs, "create gr3 2 x y");
  BOOST_CHECK(graphs.has("gr3"));
  BOOST_CHECK(graphs.at("gr3").hasVertex("x"));
  BOOST_CHECK(graphs.at("gr3").hasVertex("y"));
}

BOOST_AUTO_TEST_CASE(cmd_create_duplicate_is_invalid)
{
  borisov::GraphTable graphs = makeGraphs("gr3 0\n");
  BOOST_CHECK_EQUAL(run(graphs, "create gr3 0"), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_merge_creates_combined_graph)
{
  borisov::GraphTable graphs = makeGraphs(
    "gr1 1\na b 5\n"
    "gr2 1\nc d 7\n"
  );
  run(graphs, "merge combined gr1 gr2");
  BOOST_CHECK(graphs.has("combined"));
  auto& g = graphs.at("combined");
  BOOST_CHECK(g.hasVertex("a"));
  BOOST_CHECK(g.hasVertex("c"));
}

BOOST_AUTO_TEST_CASE(cmd_merge_invalid_source)
{
  borisov::GraphTable graphs = makeGraphs("gr1 0\n");
  BOOST_CHECK_EQUAL(run(graphs, "merge merged gr1 noGraph"), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_merge_target_exists_is_invalid)
{
  borisov::GraphTable graphs = makeGraphs("gr1 0\ngr2 0\n");
  run(graphs, "merge gr3 gr1 gr2");
  BOOST_CHECK_EQUAL(run(graphs, "merge gr3 gr1 gr2"), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_extract_creates_subgraph)
{
  borisov::GraphTable graphs = makeGraphs(
    "gr2 3\na b 1\nb b 2\na c 3\n"
  );
  run(graphs, "extract sub gr2 2 a c");
  BOOST_CHECK(graphs.has("sub"));
  std::string result = run(graphs, "outbound sub a");
  BOOST_CHECK_EQUAL(result, "c 3\n");
}

BOOST_AUTO_TEST_CASE(cmd_extract_invalid_vertex)
{
  borisov::GraphTable graphs = makeGraphs("gr2 1\na b 1\n");
  BOOST_CHECK_EQUAL(run(graphs, "extract sub gr2 1 z"), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_extract_target_exists_is_invalid)
{
  borisov::GraphTable graphs = makeGraphs("gr2 1\na b 1\ngr3 0\n");
  BOOST_CHECK_EQUAL(run(graphs, "extract gr3 gr2 1 a"), "<INVALID COMMAND>\n");
}

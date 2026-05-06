#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <sstream>
#include <string>

#include "command_parser.hpp"
#include "commands.hpp"
#include "graph.hpp"
#include "graph_storage.hpp"
#include "input_reader.hpp"

namespace
{
  std::string dispatchLine(const std::string& line, alekseev::GraphStorage& storage)
  {
    const alekseev::CommandTable commands = alekseev::makeCommandTable();
    const alekseev::Sequence< std::string > args = alekseev::splitWords(line);
    std::ostringstream out;
    alekseev::dispatchCommand(commands, args, storage, out);
    return out.str();
  }
}

BOOST_AUTO_TEST_CASE(graph_bind_cut_and_sorted_queries)
{
  alekseev::Graph graph;
  graph.bind("b", "c", 5);
  graph.bind("b", "c", 5);
  graph.bind("b", "c", 1);
  graph.bind("a", "b", 0);
  graph.addVertex("d");

  BOOST_TEST(graph.hasVertex("a"));
  BOOST_TEST(graph.hasVertex("b"));
  BOOST_TEST(graph.hasVertex("c"));
  BOOST_TEST(graph.hasVertex("d"));

  alekseev::Sequence< std::string > vertexes = graph.getSortedVertexes();
  BOOST_TEST(vertexes.size() == 4);
  BOOST_TEST(vertexes[0] == "a");
  BOOST_TEST(vertexes[1] == "b");
  BOOST_TEST(vertexes[2] == "c");
  BOOST_TEST(vertexes[3] == "d");

  alekseev::Sequence< alekseev::EdgeQueryLine > outbound = graph.getSortedOutbound("b");
  BOOST_TEST(outbound.size() == 1);
  BOOST_TEST(outbound[0].vertex == "c");
  BOOST_TEST(outbound[0].weights.size() == 3);
  BOOST_TEST(outbound[0].weights[0] == 1);
  BOOST_TEST(outbound[0].weights[1] == 5);
  BOOST_TEST(outbound[0].weights[2] == 5);

  alekseev::Sequence< alekseev::EdgeQueryLine > inbound = graph.getSortedInbound("b");
  BOOST_TEST(inbound.size() == 1);
  BOOST_TEST(inbound[0].vertex == "a");
  BOOST_TEST(inbound[0].weights.size() == 1);
  BOOST_TEST(inbound[0].weights[0] == 0);

  BOOST_TEST(graph.cut("b", "c", 5));
  outbound = graph.getSortedOutbound("b");
  BOOST_TEST(outbound[0].weights.size() == 2);
  BOOST_TEST(outbound[0].weights[0] == 1);
  BOOST_TEST(outbound[0].weights[1] == 5);
  BOOST_TEST(!graph.cut("b", "c", 7));
  BOOST_TEST(graph.cut("b", "c", 1));
  BOOST_TEST(graph.cut("b", "c", 5));
  BOOST_TEST(graph.getSortedOutbound("b").empty());
  BOOST_TEST(graph.hasVertex("b"));
  BOOST_TEST(graph.hasVertex("c"));
  BOOST_TEST(graph.getSortedInbound("d").empty());
  BOOST_TEST(graph.getSortedOutbound("d").empty());
}

BOOST_AUTO_TEST_CASE(graph_storage_operations)
{
  alekseev::GraphStorage storage;
  BOOST_TEST(storage.createGraph("b"));
  BOOST_TEST(!storage.createGraph("b"));
  BOOST_TEST(storage.hasGraph("b"));
  storage.getGraph("b").addVertex("v");
  BOOST_TEST(storage.getGraph("b").hasVertex("v"));

  alekseev::Graph graph;
  graph.addVertex("x");
  BOOST_TEST(storage.addGraph("a", graph));
  alekseev::Sequence< std::string > names = storage.getSortedGraphNames();
  BOOST_TEST(names.size() == 2);
  BOOST_TEST(names[0] == "a");
  BOOST_TEST(names[1] == "b");
}

BOOST_AUTO_TEST_CASE(input_reader_accepts_valid_graphs)
{
  {
    std::istringstream input("gr1 2\na b 0\na b 0\n");
    alekseev::GraphStorage storage;
    BOOST_TEST(alekseev::loadGraphs(input, storage));
    BOOST_TEST(storage.hasGraph("gr1"));
    const alekseev::Graph& graph = storage.getGraph("gr1");
    alekseev::Sequence< alekseev::EdgeQueryLine > outbound = graph.getSortedOutbound("a");
    BOOST_TEST(outbound[0].weights.size() == 2);
    BOOST_TEST(outbound[0].weights[0] == 0);
    BOOST_TEST(outbound[0].weights[1] == 0);
  }
  {
    std::istringstream input("\ngr1 1\na b 1\n\ngr2 1\nc d 2\n");
    alekseev::GraphStorage storage;
    BOOST_TEST(alekseev::loadGraphs(input, storage));
    BOOST_TEST(storage.hasGraph("gr1"));
    BOOST_TEST(storage.hasGraph("gr2"));
  }
}

BOOST_AUTO_TEST_CASE(input_reader_rejects_invalid_graphs)
{
  {
    std::istringstream input("gr1 x\n");
    alekseev::GraphStorage storage;
    BOOST_TEST(!alekseev::loadGraphs(input, storage));
  }
  {
    std::istringstream input("gr1 2\na b 1\n");
    alekseev::GraphStorage storage;
    BOOST_TEST(!alekseev::loadGraphs(input, storage));
  }
  {
    std::istringstream input("gr1 1\na b q\n");
    alekseev::GraphStorage storage;
    BOOST_TEST(!alekseev::loadGraphs(input, storage));
  }
  {
    std::istringstream input("gr1 0\ngr1 0\n");
    alekseev::GraphStorage storage;
    BOOST_TEST(!alekseev::loadGraphs(input, storage));
  }
}

BOOST_AUTO_TEST_CASE(command_parser_helpers)
{
  alekseev::Sequence< std::string > words = alekseev::splitWords("bind  gr a  b 0");
  BOOST_TEST(words.size() == 5);
  BOOST_TEST(words[0] == "bind");
  BOOST_TEST(words[1] == "gr");
  BOOST_TEST(words[2] == "a");
  BOOST_TEST(words[3] == "b");
  BOOST_TEST(words[4] == "0");
  BOOST_TEST(alekseev::splitWords("").empty());

  std::size_t parsedSize = 0;
  BOOST_TEST(alekseev::parseSize("42", parsedSize));
  BOOST_TEST(parsedSize == 42);
  BOOST_TEST(!alekseev::parseSize("", parsedSize));
  BOOST_TEST(!alekseev::parseSize("12a", parsedSize));

  unsigned long long parsedWeight = 1;
  BOOST_TEST(alekseev::parseUll("0", parsedWeight));
  BOOST_TEST(parsedWeight == 0);
  BOOST_TEST(!alekseev::parseUll("-1", parsedWeight));
  BOOST_TEST(!alekseev::parseUll("x", parsedWeight));
}

BOOST_AUTO_TEST_CASE(commands_dispatch_graph_queries_and_mutations)
{
  alekseev::GraphStorage storage;
  alekseev::Graph first;
  first.bind("a", "c", 3);
  first.bind("a", "b", 1);
  first.bind("c", "b", 2);
  BOOST_TEST(storage.addGraph("gr2", first));
  BOOST_TEST(storage.createGraph("gr1"));

  BOOST_TEST(dispatchLine("graphs", storage) == "gr1\ngr2\n");
  BOOST_TEST(dispatchLine("vertexes gr2", storage) == "a\nb\nc\n");
  BOOST_TEST(dispatchLine("vertexes missing", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(dispatchLine("outbound gr2 a", storage) == "b 1\nc 3\n");
  BOOST_TEST(dispatchLine("inbound gr2 b", storage) == "a 1\nc 2\n");

  BOOST_TEST(dispatchLine("bind gr2 b d 0", storage).empty());
  BOOST_TEST(storage.getGraph("gr2").hasVertex("d"));
  BOOST_TEST(dispatchLine("outbound gr2 b", storage) == "d 0\n");
  BOOST_TEST(dispatchLine("cut gr2 b d 0", storage).empty());
  BOOST_TEST(dispatchLine("cut gr2 b d 0", storage) == "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(commands_dispatch_create_merge_extract_and_invalid)
{
  alekseev::GraphStorage storage;
  alekseev::Graph first;
  first.bind("a", "c", 3);
  first.bind("a", "b", 1);
  first.bind("c", "b", 2);
  BOOST_TEST(storage.addGraph("gr2", first));
  BOOST_TEST(storage.createGraph("gr1"));

  BOOST_TEST(dispatchLine("create empty", storage).empty());
  BOOST_TEST(storage.hasGraph("empty"));
  BOOST_TEST(dispatchLine("vertexes empty", storage) == "\n");
  BOOST_TEST(dispatchLine("create zero 0", storage).empty());
  BOOST_TEST(storage.hasGraph("zero"));
  BOOST_TEST(dispatchLine("create made 3 x y z", storage).empty());
  BOOST_TEST(dispatchLine("vertexes made", storage) == "x\ny\nz\n");
  BOOST_TEST(dispatchLine("create bad 2 x y z", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(dispatchLine("create badZero 0 x", storage) == "<INVALID COMMAND>\n");

  BOOST_TEST(dispatchLine("merge merged gr2 gr2", storage).empty());
  BOOST_TEST(dispatchLine("inbound merged b", storage) == "a 1 1\nc 2 2\n");
  BOOST_TEST(dispatchLine("merge gr2 gr1 gr1", storage) == "<INVALID COMMAND>\n");

  BOOST_TEST(dispatchLine("extract sub gr2 2 a c", storage).empty());
  BOOST_TEST(dispatchLine("vertexes sub", storage) == "a\nc\n");
  BOOST_TEST(dispatchLine("outbound sub a", storage) == "c 3\n");
  BOOST_TEST(dispatchLine("extract broken gr2 2 a missing", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(dispatchLine("", storage) == "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(commands_output_formatting)
{
  std::ostringstream invalid;
  alekseev::printInvalid(invalid);
  BOOST_TEST(invalid.str() == "<INVALID COMMAND>\n");

  alekseev::GraphStorage storage;
  BOOST_TEST(storage.createGraph("g"));
  BOOST_TEST(dispatchLine("graphs", storage) == "g\n");
  BOOST_TEST(dispatchLine("bind g a b 2", storage).empty());
  const std::string outbound = dispatchLine("outbound g a", storage);
  BOOST_TEST(outbound == "b 2\n");
  BOOST_TEST(!outbound.empty());
  BOOST_TEST(outbound[outbound.size() - 1] == '\n');
  BOOST_TEST(outbound.size() >= 2);
  BOOST_TEST(outbound[outbound.size() - 2] != ' ');
  BOOST_TEST(dispatchLine("outbound g b", storage) == "\n");
  BOOST_TEST(dispatchLine("inbound g a", storage) == "\n");

  alekseev::GraphStorage emptyStorage;
  BOOST_TEST(dispatchLine("graphs", emptyStorage) == "\n");
}

#include "commands.hpp"

#include <cstddef>
#include <string>

#include "command_parser.hpp"

namespace alekseev
{
  namespace
  {
    const size_t CommandTableSlots = 23;
    const size_t CommandNameIndex = 0;
    const size_t GraphNameIndex = 1;
    const size_t VertexIndex = 2;
    const size_t FromVertexIndex = 2;
    const size_t ToVertexIndex = 3;
    const size_t WeightIndex = 4;
    const size_t CountIndex = 2;
    const size_t ExtractCountIndex = 3;
    const size_t CreateVertexBegin = 3;
    const size_t ExtractVertexBegin = 4;
    const size_t GraphsArgs = 1;
    const size_t CreateEmptyArgs = 2;
    const size_t VertexesArgs = 2;
    const size_t EdgeQueryArgs = 3;
    const size_t ExtractMinArgs = 4;
    const size_t MergeArgs = 4;
    const size_t EdgeMutationArgs = 5;

    bool isValidNameLocal(const std::string& name)
    {
      return detail::isValidName(name);
    }

    void printStrings(const Sequence< std::string >& values, std::ostream& out)
    {
      if (values.empty())
      {
        out << '\n';
        return;
      }
      for (size_t i = 0; i < values.size(); ++i)
      {
        out << values[i] << '\n';
      }
    }

    void printEdgeLine(const EdgeQueryLine& line, std::ostream& out)
    {
      out << line.vertex;
      for (size_t i = 0; i < line.weights.size(); ++i)
      {
        out << ' ' << line.weights[i];
      }
      out << '\n';
    }

    void printEdgeLines(const Sequence< EdgeQueryLine >& lines, std::ostream& out)
    {
      if (lines.empty())
      {
        out << '\n';
        return;
      }
      for (size_t i = 0; i < lines.size(); ++i)
      {
        printEdgeLine(lines[i], out);
      }
    }

    bool containsString(const Sequence< std::string >& values, const std::string& value)
    {
      for (size_t i = 0; i < values.size(); ++i)
      {
        if (values[i] == value)
        {
          return true;
        }
      }
      return false;
    }

    void copyGraphInto(const Graph& source, Graph& target)
    {
      Sequence< std::string > vertexes = source.getSortedVertexes();
      for (size_t i = 0; i < vertexes.size(); ++i)
      {
        target.addVertex(vertexes[i]);
      }
      for (typename EdgeTable::const_iterator it = source.edges().begin();
          it != source.edges().end(); ++it)
      {
        for (size_t i = 0; i < it->value().size(); ++i)
        {
          target.bind(it->key().from, it->key().to, it->value()[i]);
        }
      }
    }
  }

  void handleGraphs(const Sequence< std::string >& args, GraphStorage& storage, std::ostream& out)
  {
    if (args.size() != GraphsArgs)
    {
      printInvalid(out);
      return;
    }
    printStrings(storage.getSortedGraphNames(), out);
  }

  void handleVertexes(const Sequence< std::string >& args, GraphStorage& storage, std::ostream& out)
  {
    if (args.size() != VertexesArgs || !storage.hasGraph(args[GraphNameIndex]))
    {
      printInvalid(out);
      return;
    }
    printStrings(storage.getGraph(args[GraphNameIndex]).getSortedVertexes(), out);
  }

  void handleOutbound(const Sequence< std::string >& args, GraphStorage& storage, std::ostream& out)
  {
    if (args.size() != EdgeQueryArgs || !storage.hasGraph(args[GraphNameIndex]))
    {
      printInvalid(out);
      return;
    }
    const Graph& graph = storage.getGraph(args[GraphNameIndex]);
    if (!graph.hasVertex(args[VertexIndex]))
    {
      printInvalid(out);
      return;
    }
    printEdgeLines(graph.getSortedOutbound(args[VertexIndex]), out);
  }

  void handleInbound(const Sequence< std::string >& args, GraphStorage& storage, std::ostream& out)
  {
    if (args.size() != EdgeQueryArgs || !storage.hasGraph(args[GraphNameIndex]))
    {
      printInvalid(out);
      return;
    }
    const Graph& graph = storage.getGraph(args[GraphNameIndex]);
    if (!graph.hasVertex(args[VertexIndex]))
    {
      printInvalid(out);
      return;
    }
    printEdgeLines(graph.getSortedInbound(args[VertexIndex]), out);
  }

  void handleBind(const Sequence< std::string >& args, GraphStorage& storage, std::ostream& out)
  {
    if (args.size() != EdgeMutationArgs || !storage.hasGraph(args[GraphNameIndex]) ||
        !isValidNameLocal(args[FromVertexIndex]) || !isValidNameLocal(args[ToVertexIndex]))
    {
      printInvalid(out);
      return;
    }
    unsigned long long weight = 0;
    if (!parseUll(args[WeightIndex], weight))
    {
      printInvalid(out);
      return;
    }
    storage.getGraph(args[GraphNameIndex]).bind(
        args[FromVertexIndex], args[ToVertexIndex], weight);
  }

  void handleCut(const Sequence< std::string >& args, GraphStorage& storage, std::ostream& out)
  {
    if (args.size() != EdgeMutationArgs || !storage.hasGraph(args[GraphNameIndex]))
    {
      printInvalid(out);
      return;
    }
    unsigned long long weight = 0;
    if (!parseUll(args[WeightIndex], weight))
    {
      printInvalid(out);
      return;
    }
    if (!storage.getGraph(args[GraphNameIndex]).cut(
        args[FromVertexIndex], args[ToVertexIndex], weight))
    {
      printInvalid(out);
    }
  }

  void handleCreate(const Sequence< std::string >& args, GraphStorage& storage, std::ostream& out)
  {
    if (args.size() < CreateEmptyArgs ||
        !isValidNameLocal(args[GraphNameIndex]) || storage.hasGraph(args[GraphNameIndex]))
    {
      printInvalid(out);
      return;
    }

    Graph graph;
    if (args.size() > 2)
    {
      size_t count = 0;
      if (!parseSize(args[CountIndex], count) || count != args.size() - CreateVertexBegin)
      {
        printInvalid(out);
        return;
      }
      for (size_t i = CreateVertexBegin; i < args.size(); ++i)
      {
        if (!isValidNameLocal(args[i]))
        {
          printInvalid(out);
          return;
        }
        graph.addVertex(args[i]);
      }
    }
    if (!storage.addGraph(args[GraphNameIndex], graph))
    {
      printInvalid(out);
    }
  }

  void handleMerge(const Sequence< std::string >& args, GraphStorage& storage, std::ostream& out)
  {
    if (args.size() != MergeArgs || !isValidNameLocal(args[GraphNameIndex]) ||
        storage.hasGraph(args[GraphNameIndex]) || !storage.hasGraph(args[FromVertexIndex]) ||
        !storage.hasGraph(args[ToVertexIndex]))
    {
      printInvalid(out);
      return;
    }
    const Graph& first = storage.getGraph(args[FromVertexIndex]);
    const Graph& second = storage.getGraph(args[ToVertexIndex]);
    Graph merged(first.edges().size() + second.edges().size());
    copyGraphInto(first, merged);
    copyGraphInto(second, merged);
    if (!storage.addGraph(args[GraphNameIndex], merged))
    {
      printInvalid(out);
    }
  }

  void handleExtract(const Sequence< std::string >& args, GraphStorage& storage, std::ostream& out)
  {
    if (args.size() < ExtractMinArgs || !isValidNameLocal(args[GraphNameIndex]) ||
        storage.hasGraph(args[GraphNameIndex]) || !storage.hasGraph(args[VertexIndex]))
    {
      printInvalid(out);
      return;
    }
    size_t count = 0;
    if (!parseSize(args[ExtractCountIndex], count) ||
        count != args.size() - ExtractVertexBegin)
    {
      printInvalid(out);
      return;
    }

    const Graph& source = storage.getGraph(args[VertexIndex]);
    Sequence< std::string > selected;
    for (size_t i = ExtractVertexBegin; i < args.size(); ++i)
    {
      if (!source.hasVertex(args[i]))
      {
        printInvalid(out);
        return;
      }
      selected.push_back(args[i]);
    }

    Graph extracted(source.edges().size());
    for (size_t i = 0; i < selected.size(); ++i)
    {
      extracted.addVertex(selected[i]);
    }
    for (typename EdgeTable::const_iterator it = source.edges().begin();
        it != source.edges().end(); ++it)
    {
      if (containsString(selected, it->key().from) && containsString(selected, it->key().to))
      {
        for (size_t i = 0; i < it->value().size(); ++i)
        {
          extracted.bind(it->key().from, it->key().to, it->value()[i]);
        }
      }
    }

    if (!storage.addGraph(args[GraphNameIndex], extracted))
    {
      printInvalid(out);
    }
  }

  CommandTable makeCommandTable()
  {
    CommandTable commands(CommandTableSlots);
    commands.add("graphs", handleGraphs);
    commands.add("vertexes", handleVertexes);
    commands.add("outbound", handleOutbound);
    commands.add("inbound", handleInbound);
    commands.add("bind", handleBind);
    commands.add("cut", handleCut);
    commands.add("create", handleCreate);
    commands.add("merge", handleMerge);
    commands.add("extract", handleExtract);
    return commands;
  }

  void dispatchCommand(const CommandTable& commands, const Sequence< std::string >& args,
      GraphStorage& storage, std::ostream& out)
  {
    if (args.empty() || !commands.has(args[CommandNameIndex]))
    {
      printInvalid(out);
      return;
    }
    commands.at(args[CommandNameIndex])(args, storage, out);
  }
}

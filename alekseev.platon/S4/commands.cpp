#include "commands.hpp"

#include <stdexcept>

#include <line_parser.hpp>

namespace alekseev
{
  namespace
  {
    const char PrintCommand[] = "print";
    const char ComplementCommand[] = "complement";
    const char IntersectCommand[] = "intersect";
    const char UnionCommand[] = "union";

    struct SetOperationArgs
    {
      std::string resultName;
      std::string leftName;
      std::string rightName;
    };

    SetOperationArgs readSetOperationArgs(const std::string& line)
    {
      LineParser parser(line);
      parser.readWord();
      SetOperationArgs args{parser.readWord(), parser.readWord(), parser.readWord()};
      parser.expectEnd();
      return args;
    }

    bool hasValidSetOperationArgs(const SetOperationArgs& args, const DictionaryStorage& storage)
    {
      return storage.contains(args.leftName) && storage.contains(args.rightName);
    }

    void printDictionary(const std::string& name, const Dictionary& dictionary,
        std::ostream& out)
    {
      if (dictionary.empty())
      {
        out << EmptyDictionary << '\n';
        return;
      }

      out << name;
      for (Dictionary::const_iterator it = dictionary.cbegin(); it != dictionary.cend(); ++it)
      {
        out << ' ' << it->first << ' ' << it->second;
      }
      out << '\n';
    }

    void handlePrint(const std::string& line, DictionaryStorage& storage, std::ostream& out)
    {
      LineParser parser(line);
      parser.readWord();
      const std::string name = parser.readWord();
      parser.expectEnd();
      if (!storage.contains(name))
      {
        printInvalid(out);
        return;
      }
      printDictionary(name, storage.get(name), out);
    }

    void handleComplement(const std::string& line, DictionaryStorage& storage, std::ostream& out)
    {
      const SetOperationArgs args = readSetOperationArgs(line);
      if (!hasValidSetOperationArgs(args, storage))
      {
        printInvalid(out);
        return;
      }

      const Dictionary& left = storage.get(args.leftName);
      const Dictionary& right = storage.get(args.rightName);
      Dictionary result;
      for (Dictionary::const_iterator it = left.cbegin(); it != left.cend(); ++it)
      {
        if (!right.contains(it->first))
        {
          result.push(it->first, it->second);
        }
      }
      storage.push(args.resultName, result);
    }

    void handleIntersect(const std::string& line, DictionaryStorage& storage, std::ostream& out)
    {
      const SetOperationArgs args = readSetOperationArgs(line);
      if (!hasValidSetOperationArgs(args, storage))
      {
        printInvalid(out);
        return;
      }

      const Dictionary& left = storage.get(args.leftName);
      const Dictionary& right = storage.get(args.rightName);
      Dictionary result;
      for (Dictionary::const_iterator it = left.cbegin(); it != left.cend(); ++it)
      {
        if (right.contains(it->first))
        {
          result.push(it->first, it->second);
        }
      }
      storage.push(args.resultName, result);
    }

    void handleUnion(const std::string& line, DictionaryStorage& storage, std::ostream& out)
    {
      const SetOperationArgs args = readSetOperationArgs(line);
      if (!hasValidSetOperationArgs(args, storage))
      {
        printInvalid(out);
        return;
      }

      const Dictionary& left = storage.get(args.leftName);
      const Dictionary& right = storage.get(args.rightName);
      Dictionary result(left);
      for (Dictionary::const_iterator it = right.cbegin(); it != right.cend(); ++it)
      {
        if (!result.contains(it->first))
        {
          result.push(it->first, it->second);
        }
      }
      storage.push(args.resultName, result);
    }
  }

  void printInvalid(std::ostream& out)
  {
    out << InvalidCommand << '\n';
  }

  CommandTable makeCommandTable()
  {
    CommandTable commands;
    commands.push(PrintCommand, handlePrint);
    commands.push(ComplementCommand, handleComplement);
    commands.push(IntersectCommand, handleIntersect);
    commands.push(UnionCommand, handleUnion);
    return commands;
  }

  void dispatchCommand(const CommandTable& commands, const std::string& line,
      DictionaryStorage& storage, std::ostream& out)
  {
    try
    {
      LineParser parser(line);
      if (!parser.hasNext())
      {
        return;
      }
      const std::string command = parser.readWord();
      if (!commands.contains(command))
      {
        printInvalid(out);
        return;
      }
      commands.get(command)(line, storage, out);
    }
    catch (const std::exception&)
    {
      printInvalid(out);
    }
  }
}

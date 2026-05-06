#ifndef ALEKSEEV_S4_COMMANDS_HPP
#define ALEKSEEV_S4_COMMANDS_HPP

#include <ostream>
#include <string>

#include "dataset.hpp"

namespace alekseev
{
  const char InvalidCommand[] = "<INVALID COMMAND>";
  const char EmptyDictionary[] = "<EMPTY>";

  using CommandHandler = void (*)(const std::string&, DictionaryStorage&, std::ostream&);
  using CommandTable = BSTree< std::string, CommandHandler >;

  void printInvalid(std::ostream& out);
  CommandTable makeCommandTable();
  void dispatchCommand(const CommandTable& commands, const std::string& line,
      DictionaryStorage& storage, std::ostream& out);
}

#endif

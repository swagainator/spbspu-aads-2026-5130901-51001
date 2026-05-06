#ifndef ALEKSEEV_S3_COMMANDS_HPP
#define ALEKSEEV_S3_COMMANDS_HPP

#include <ostream>
#include <string>

#include "graph_storage.hpp"
#include <hash_table.hpp>
#include "hmac_hash.hpp"
#include <sequence.hpp>

namespace alekseev
{
  using CommandHandler = void (*)(const Sequence< std::string >&, GraphStorage&, std::ostream&);
  using CommandTable = HashTable< std::string, CommandHandler, HmacHash, StringEqual >;

  inline void printInvalid(std::ostream& out)
  {
    out << "<INVALID COMMAND>\n";
  }

  void handleGraphs(const Sequence< std::string >&, GraphStorage&, std::ostream&);
  void handleVertexes(const Sequence< std::string >&, GraphStorage&, std::ostream&);
  void handleOutbound(const Sequence< std::string >&, GraphStorage&, std::ostream&);
  void handleInbound(const Sequence< std::string >&, GraphStorage&, std::ostream&);
  void handleBind(const Sequence< std::string >&, GraphStorage&, std::ostream&);
  void handleCut(const Sequence< std::string >&, GraphStorage&, std::ostream&);
  void handleCreate(const Sequence< std::string >&, GraphStorage&, std::ostream&);
  void handleMerge(const Sequence< std::string >&, GraphStorage&, std::ostream&);
  void handleExtract(const Sequence< std::string >&, GraphStorage&, std::ostream&);

  CommandTable makeCommandTable();
  void dispatchCommand(const CommandTable& commands, const Sequence< std::string >& args,
      GraphStorage& storage, std::ostream& out);
}

#endif

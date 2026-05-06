#ifndef ALEKSEEV_S3_GRAPH_STORAGE_HPP
#define ALEKSEEV_S3_GRAPH_STORAGE_HPP

#include <cstddef>
#include <string>

#include "graph.hpp"
#include <hash_table.hpp>
#include "hmac_hash.hpp"
#include <sequence.hpp>
#include <sort_utils.hpp>

namespace alekseev
{
  namespace detail
  {
    const size_t DefaultGraphStorageSlots = 1009;
  }

  class GraphStorage
  {
  public:
    GraphStorage():
      graphs_(detail::DefaultGraphStorageSlots)
    {
    }

    bool hasGraph(const std::string& name) const
    {
      return graphs_.has(name);
    }

    Graph& getGraph(const std::string& name)
    {
      return graphs_.at(name);
    }

    const Graph& getGraph(const std::string& name) const
    {
      return graphs_.at(name);
    }

    bool createGraph(const std::string& name)
    {
      if (hasGraph(name))
      {
        return false;
      }
      graphs_.add(name, Graph());
      return true;
    }

    bool addGraph(const std::string& name, const Graph& graph)
    {
      if (hasGraph(name))
      {
        return false;
      }
      graphs_.add(name, graph);
      return true;
    }

    Sequence< std::string > getSortedGraphNames() const
    {
      Sequence< std::string > result;
      for (typename GraphTable::const_iterator it = graphs_.begin(); it != graphs_.end();
          ++it)
      {
        result.push_back(it->key());
      }
      sort(result, StringLess());
      return result;
    }

  private:
    using GraphTable = HashTable< std::string, Graph, HmacHash, StringEqual >;

    struct StringLess
    {
      bool operator()(const std::string& lhs, const std::string& rhs) const
      {
        return lhs < rhs;
      }
    };

    GraphTable graphs_;
  };
}

#endif

#ifndef ALEKSEEV_S3_GRAPH_HPP
#define ALEKSEEV_S3_GRAPH_HPP

#include <cstddef>
#include <string>

#include "edge_key.hpp"
#include <hash_table.hpp>
#include "hmac_hash.hpp"
#include <sequence.hpp>
#include <sort_utils.hpp>

namespace alekseev
{
  using EdgeTable = HashTable< EdgeKey, Sequence< unsigned long long >, HmacHash, EdgeKeyEqual >;

  namespace detail
  {
    const size_t DefaultEdgeSlots = 211;
    const size_t MinEdgeSlots = 11;
    const size_t EdgeCapacityMultiplier = 2;
    const size_t EdgeCapacityReserve = 3;
    const size_t MakeOddStep = 1;
  }

  struct EdgeQueryLine
  {
    std::string vertex;
    Sequence< unsigned long long > weights;
  };

  class Graph
  {
  public:
    Graph():
      vertexes_(),
      edges_(detail::DefaultEdgeSlots)
    {
    }

    explicit Graph(size_t expected_edges):
      vertexes_(),
      edges_(getNextOddCapacity(
          expected_edges * detail::EdgeCapacityMultiplier + detail::EdgeCapacityReserve))
    {
    }

    void addVertex(const std::string& name)
    {
      if (!hasVertex(name))
      {
        vertexes_.push_back(name);
      }
    }

    bool hasVertex(const std::string& name) const
    {
      return containsVertex(vertexes_, name);
    }

    void bind(const std::string& from, const std::string& to, unsigned long long weight)
    {
      addVertex(from);
      addVertex(to);
      EdgeKey key{from, to};
      if (edges_.has(key))
      {
        edges_.at(key).push_back(weight);
      }
      else
      {
        Sequence< unsigned long long > weights;
        weights.push_back(weight);
        edges_.add(key, weights);
      }
    }

    bool cut(const std::string& from, const std::string& to, unsigned long long weight)
    {
      if (!hasVertex(from) || !hasVertex(to))
      {
        return false;
      }
      EdgeKey key{from, to};
      if (!edges_.has(key))
      {
        return false;
      }
      Sequence< unsigned long long >& weights = edges_.at(key);
      for (size_t i = 0; i < weights.size(); ++i)
      {
        if (weights[i] == weight)
        {
          weights.erase_at(i);
          if (weights.empty())
          {
            edges_.drop(key);
          }
          return true;
        }
      }
      return false;
    }

    Sequence< std::string > getSortedVertexes() const
    {
      Sequence< std::string > result(vertexes_);
      sort(result, StringLess());
      return result;
    }

    Sequence< EdgeQueryLine > getSortedOutbound(const std::string& vertex) const
    {
      Sequence< EdgeQueryLine > result;
      for (typename EdgeTable::const_iterator it = edges_.begin(); it != edges_.end();
          ++it)
      {
        if (it->key().from == vertex)
        {
          appendQueryLine(result, it->key().to, it->value());
        }
      }
      sortQueryLines(result);
      return result;
    }

    Sequence< EdgeQueryLine > getSortedInbound(const std::string& vertex) const
    {
      Sequence< EdgeQueryLine > result;
      for (typename EdgeTable::const_iterator it = edges_.begin(); it != edges_.end();
          ++it)
      {
        if (it->key().to == vertex)
        {
          appendQueryLine(result, it->key().from, it->value());
        }
      }
      sortQueryLines(result);
      return result;
    }

    const EdgeTable& edges() const
    {
      return edges_;
    }

  private:
    struct StringLess
    {
      bool operator()(const std::string& lhs, const std::string& rhs) const
      {
        return lhs < rhs;
      }
    };

    struct ULLLess
    {
      bool operator()(unsigned long long lhs, unsigned long long rhs) const
      {
        return lhs < rhs;
      }
    };

    struct QueryLineLess
    {
      bool operator()(const EdgeQueryLine& lhs, const EdgeQueryLine& rhs) const
      {
        return lhs.vertex < rhs.vertex;
      }
    };

    static size_t getNextOddCapacity(size_t value)
    {
      if (value < detail::MinEdgeSlots)
      {
        return detail::MinEdgeSlots;
      }
      return value % detail::EdgeCapacityMultiplier == 0 ? value + detail::MakeOddStep : value;
    }

    static bool containsVertex(const Sequence< std::string >& seq, const std::string& name)
    {
      for (size_t i = 0; i < seq.size(); ++i)
      {
        if (seq[i] == name)
        {
          return true;
        }
      }
      return false;
    }

    static void appendQueryLine(Sequence< EdgeQueryLine >& result, const std::string& vertex,
        const Sequence< unsigned long long >& weights)
    {
      EdgeQueryLine line;
      line.vertex = vertex;
      line.weights = weights;
      sort(line.weights, ULLLess());
      result.push_back(line);
    }

    static void sortQueryLines(Sequence< EdgeQueryLine >& lines)
    {
      sort(lines, QueryLineLess());
    }

    Sequence< std::string > vertexes_;
    EdgeTable edges_;
  };
}

#endif

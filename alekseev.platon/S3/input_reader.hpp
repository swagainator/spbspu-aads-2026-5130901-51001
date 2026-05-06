#ifndef ALEKSEEV_S3_INPUT_READER_HPP
#define ALEKSEEV_S3_INPUT_READER_HPP

#include <cstddef>
#include <istream>
#include <limits>
#include <string>

#include "graph.hpp"
#include "graph_storage.hpp"

namespace alekseev
{
  namespace detail
  {
    const size_t DecimalBase = 10;
    const size_t GraphHeaderWords = 2;
    const size_t EdgeWords = 3;
    const size_t GraphNameWord = 0;
    const size_t EdgeCountWord = 1;
    const size_t FromWord = 0;
    const size_t ToWord = 1;
    const size_t WeightWord = 2;
    const size_t FirstNameChar = 1;

    inline bool isDigit(char c)
    {
      return c >= '0' && c <= '9';
    }

    inline bool isAlpha(char c)
    {
      return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    inline bool isAlnum(char c)
    {
      return isAlpha(c) || isDigit(c);
    }

    inline bool isValidName(const std::string& value)
    {
      if (value.empty() || !isAlpha(value[0]))
      {
        return false;
      }
      for (size_t i = FirstNameChar; i < value.size(); ++i)
      {
        if (!isAlnum(value[i]))
        {
          return false;
        }
      }
      return true;
    }

    inline bool parseSize(const std::string& value, size_t& result)
    {
      if (value.empty())
      {
        return false;
      }
      size_t current = 0;
      for (size_t i = 0; i < value.size(); ++i)
      {
        if (!isDigit(value[i]))
        {
          return false;
        }
        const size_t digit = static_cast< size_t >(value[i] - '0');
        if (current > (std::numeric_limits< size_t >::max() - digit) / DecimalBase)
        {
          return false;
        }
        current = current * DecimalBase + digit;
      }
      result = current;
      return true;
    }

    inline bool parseUll(const std::string& value, unsigned long long& result)
    {
      if (value.empty())
      {
        return false;
      }
      unsigned long long current = 0;
      for (size_t i = 0; i < value.size(); ++i)
      {
        if (!isDigit(value[i]))
        {
          return false;
        }
        const unsigned long long digit = static_cast< unsigned long long >(value[i] - '0');
        if (current >
            (std::numeric_limits< unsigned long long >::max() - digit) / DecimalBase)
        {
          return false;
        }
        current = current * DecimalBase + digit;
      }
      result = current;
      return true;
    }

    inline bool splitExact(const std::string& line, Sequence< std::string >& words)
    {
      words.clear();
      if (line.empty())
      {
        return true;
      }
      size_t start = 0;
      while (start <= line.size())
      {
        size_t end = start;
        while (end < line.size() && line[end] != ' ')
        {
          ++end;
        }
        if (end == start)
        {
          return false;
        }
        words.push_back(line.substr(start, end - start));
        if (end == line.size())
        {
          return true;
        }
        ++end;
        start = end;
      }
      return true;
    }
  }

  inline bool loadGraphs(std::istream& input, GraphStorage& storage)
  {
    std::string line;
    Sequence< std::string > words;
    while (std::getline(input, line))
    {
      if (line.empty())
      {
        continue;
      }
      if (!detail::splitExact(line, words) || words.size() != detail::GraphHeaderWords ||
          !detail::isValidName(words[detail::GraphNameWord]))
      {
        return false;
      }
      size_t edgeCount = 0;
      if (!detail::parseSize(words[detail::EdgeCountWord], edgeCount))
      {
        return false;
      }

      const std::string graphName = words[detail::GraphNameWord];
      Graph graph(edgeCount);
      for (size_t i = 0; i < edgeCount; ++i)
      {
        if (!std::getline(input, line))
        {
          return false;
        }
        while (line.empty())
        {
          if (!std::getline(input, line))
          {
            return false;
          }
        }
        if (!detail::splitExact(line, words) || words.size() != detail::EdgeWords ||
            !detail::isValidName(words[detail::FromWord]) ||
            !detail::isValidName(words[detail::ToWord]))
        {
          return false;
        }
        unsigned long long weight = 0;
        if (!detail::parseUll(words[detail::WeightWord], weight))
        {
          return false;
        }
        graph.bind(words[detail::FromWord], words[detail::ToWord], weight);
      }

      if (!storage.addGraph(graphName, graph))
      {
        return false;
      }
    }
    return true;
  }
}

#endif

#ifndef ALEKSEEV_S3_COMMAND_PARSER_HPP
#define ALEKSEEV_S3_COMMAND_PARSER_HPP

#include <cstddef>
#include <string>

#include "input_reader.hpp"
#include <sequence.hpp>

namespace alekseev
{
  namespace detail
  {
    const char CommandSeparator = ' ';
  }

  inline Sequence< std::string > splitWords(const std::string& line)
  {
    Sequence< std::string > words;
    size_t pos = 0;
    while (pos < line.size())
    {
      while (pos < line.size() && line[pos] == detail::CommandSeparator)
      {
        ++pos;
      }
      if (pos == line.size())
      {
        break;
      }
      const size_t start = pos;
      while (pos < line.size() && line[pos] != detail::CommandSeparator)
      {
        ++pos;
      }
      words.push_back(line.substr(start, pos - start));
    }
    return words;
  }

  inline bool parseSize(const std::string& value, size_t& result)
  {
    return detail::parseSize(value, result);
  }

  inline bool parseUll(const std::string& value, unsigned long long& result)
  {
    return detail::parseUll(value, result);
  }
}

#endif

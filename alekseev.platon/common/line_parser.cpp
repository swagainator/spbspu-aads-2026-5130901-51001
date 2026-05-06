#include <line_parser.hpp>

#include <cctype>
#include <climits>
#include <stdexcept>

namespace alekseev
{
  LineParser::LineParser(const std::string& line):
    line_(line),
    pos_(0)
  {
  }

  bool LineParser::hasNext() const
  {
    size_t pos = pos_;
    while (pos < line_.size() && std::isspace(static_cast< unsigned char >(line_[pos])))
    {
      ++pos;
    }
    return pos < line_.size();
  }

  std::string LineParser::readWord()
  {
    skipSpaces();
    if (pos_ == line_.size())
    {
      throw std::invalid_argument("missing token");
    }
    const size_t begin = pos_;
    while (pos_ < line_.size() && !std::isspace(static_cast< unsigned char >(line_[pos_])))
    {
      ++pos_;
    }
    return line_.substr(begin, pos_ - begin);
  }

  int LineParser::readInt()
  {
    const std::string token = readWord();
    size_t pos = 0;
    bool negative = false;
    if (token[pos] == '-' || token[pos] == '+')
    {
      negative = token[pos] == '-';
      ++pos;
    }
    if (pos == token.size())
    {
      throw std::invalid_argument("bad int");
    }

    long long value = 0;
    const long long limit = negative ? 1LL + INT_MAX : INT_MAX;
    for (; pos < token.size(); ++pos)
    {
      const unsigned char ch = static_cast< unsigned char >(token[pos]);
      if (!std::isdigit(ch))
      {
        throw std::invalid_argument("bad int");
      }
      value = value * 10 + (token[pos] - '0');
      if (value > limit)
      {
        throw std::invalid_argument("int overflow");
      }
    }
    return negative ? static_cast< int >(-value) : static_cast< int >(value);
  }

  void LineParser::expectEnd() const
  {
    if (hasNext())
    {
      throw std::invalid_argument("extra token");
    }
  }

  void LineParser::skipSpaces()
  {
    while (pos_ < line_.size() && std::isspace(static_cast< unsigned char >(line_[pos_])))
    {
      ++pos_;
    }
  }
}

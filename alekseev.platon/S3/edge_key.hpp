#ifndef ALEKSEEV_S3_EDGE_KEY_HPP
#define ALEKSEEV_S3_EDGE_KEY_HPP

#include <string>

namespace alekseev
{
  struct EdgeKey
  {
    std::string from;
    std::string to;
  };

  struct StringEqual
  {
    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
      return lhs == rhs;
    }
  };

  struct EdgeKeyEqual
  {
    bool operator()(const EdgeKey& lhs, const EdgeKey& rhs) const
    {
      return lhs.from == rhs.from && lhs.to == rhs.to;
    }
  };
}

#endif

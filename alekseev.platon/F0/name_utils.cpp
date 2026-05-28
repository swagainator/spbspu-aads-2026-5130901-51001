#include "name_utils.hpp"

#include <cstddef>

namespace alekseev
{
  namespace
  {
    bool isDigit(char c)
    {
      return c >= '0' && c <= '9';
    }

    bool isAlpha(char c)
    {
      return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    bool isAlnum(char c)
    {
      return isAlpha(c) || isDigit(c);
    }
  }

  bool isValidName(const std::string& name)
  {
    if (name.empty() || !isAlpha(name[0]))
    {
      return false;
    }
    for (std::size_t i = 1; i < name.size(); ++i)
    {
      if (!isAlnum(name[i]))
      {
        return false;
      }
    }
    return true;
  }
}

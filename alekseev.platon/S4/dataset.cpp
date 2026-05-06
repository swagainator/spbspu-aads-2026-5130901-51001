#include "dataset.hpp"

#include <istream>
#include <stdexcept>
#include <string>

#include <line_parser.hpp>

namespace alekseev
{
  bool loadDatasets(std::istream& input, DictionaryStorage& storage)
  {
    std::string line;
    while (std::getline(input, line))
    {
      try
      {
        LineParser parser(line);
        if (!parser.hasNext())
        {
          continue;
        }
        const std::string name = parser.readWord();
        if (storage.contains(name))
        {
          return false;
        }

        Dictionary dictionary;
        while (parser.hasNext())
        {
          const int key = parser.readInt();
          const std::string value = parser.readWord();
          dictionary.push(key, value);
        }
        storage.push(name, dictionary);
      }
      catch (const std::invalid_argument&)
      {
        return false;
      }
    }
    return true;
  }
}

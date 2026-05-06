#ifndef ALEKSEEV_S4_DATASET_HPP
#define ALEKSEEV_S4_DATASET_HPP

#include <iosfwd>
#include <string>

#include <bstree.hpp>

namespace alekseev
{
  using Dictionary = BSTree< int, std::string >;
  using DictionaryStorage = BSTree< std::string, Dictionary >;

  bool loadDatasets(std::istream& input, DictionaryStorage& storage);
}

#endif

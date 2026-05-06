#ifndef ALEKSEEV_SORT_UTILS_HPP
#define ALEKSEEV_SORT_UTILS_HPP

#include <cstddef>
#include <utility>

#include <sequence.hpp>

namespace alekseev
{
  namespace detail
  {
    const size_t FirstUnsortedIndex = 1;
  }

  template< class T, class Compare >
  void sort(Sequence< T >& seq, Compare cmp)
  {
    for (size_t i = detail::FirstUnsortedIndex; i < seq.size(); ++i)
    {
      T value(std::move(seq[i]));
      size_t j = i;
      while (j > 0 && cmp(value, seq[j - 1]))
      {
        seq[j] = std::move(seq[j - 1]);
        --j;
      }
      seq[j] = std::move(value);
    }
  }
}

#endif

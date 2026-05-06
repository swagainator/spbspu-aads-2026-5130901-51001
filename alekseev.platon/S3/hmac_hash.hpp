#ifndef ALEKSEEV_S3_HMAC_HASH_HPP
#define ALEKSEEV_S3_HMAC_HASH_HPP

#include <cstddef>
#include <cstdint>
#include <string>

#if __has_include(<boost/hash2/hmac.hpp>) && \
    __has_include(<boost/hash2/sha2.hpp>) && \
    __has_include(<boost/hash2/get_integral_result.hpp>)
#include <boost/hash2/get_integral_result.hpp>
#include <boost/hash2/hmac.hpp>
#include <boost/hash2/sha2.hpp>
#define ALEKSEEV_S3_HAS_BOOST_HASH2 1
#else
#define ALEKSEEV_S3_HAS_BOOST_HASH2 0
#endif

#include "edge_key.hpp"
#include <sequence.hpp>

namespace alekseev
{
  namespace detail
  {
    const size_t ByteBits = 8;
    const size_t LowByteMask = 0xffu;
    const size_t HashShift = 32;
    const unsigned long long FallbackHashSeed = 1469598103934665603ull;
    const unsigned long long FallbackHashPrime = 1099511628211ull;
    const unsigned long long FallbackInnerPad = 0x3636363636363636ull;
    const unsigned long long FallbackOuterPad = 0x5c5c5c5c5c5c5c5cull;
  }

  class HmacHash
  {
  public:
    size_t hashBytes(const unsigned char* data, size_t size) const
    {
#if ALEKSEEV_S3_HAS_BOOST_HASH2
      static const unsigned char key[] = {
        'a', 'l', 'e', 'k', 's', 'e', 'e', 'v', '-', 's', '3', '-', 'h', 'm', 'a', 'c'
      };
      boost::hash2::hmac< boost::hash2::sha2_256 > hash(key, sizeof(key));
      hash.update(data, size);
      return boost::hash2::get_integral_result< size_t >(hash);
#else
      return getFallbackHmacLike(data, size);
#endif
    }

    size_t operator()(const std::string& value) const
    {
      const unsigned char* bytes = reinterpret_cast< const unsigned char* >(value.data());
      return hashBytes(bytes, value.size());
    }

    size_t operator()(const EdgeKey& key) const
    {
      HmacHashCombiner combiner;
      combiner.appendSize(key.from.size());
      combiner.appendString(key.from);
      combiner.appendSize(key.to.size());
      combiner.appendString(key.to);
      return combiner.result(*this);
    }

  private:
    class HmacHashCombiner
    {
    public:
      HmacHashCombiner():
        data_(),
        size_(0)
      {
      }

      void appendSize(size_t value)
      {
        for (size_t i = 0; i < sizeof(size_t); ++i)
        {
          appendByte(static_cast< unsigned char >(
              (value >> (i * detail::ByteBits)) & detail::LowByteMask));
        }
      }

      void appendString(const std::string& value)
      {
        for (size_t i = 0; i < value.size(); ++i)
        {
          appendByte(static_cast< unsigned char >(value[i]));
        }
      }

      size_t result(const HmacHash& hash) const
      {
        return hash.hashBytes(data_.data(), size_);
      }

    private:
      void appendByte(unsigned char byte)
      {
        data_.push_back(byte);
        ++size_;
      }

      Sequence< unsigned char > data_;
      size_t size_;
    };

    static size_t getByteHash(const unsigned char* data, size_t size, size_t seed)
    {
      size_t result = seed;
      for (size_t i = 0; i < size; ++i)
      {
        result ^= static_cast< size_t >(data[i]);
        result *= static_cast< size_t >(detail::FallbackHashPrime);
        result ^= result >> detail::HashShift;
      }
      return result;
    }

    static size_t getFallbackHmacLike(const unsigned char* data, size_t size)
    {
      const size_t ipad = static_cast< size_t >(detail::FallbackInnerPad);
      const size_t opad = static_cast< size_t >(detail::FallbackOuterPad);
      const size_t seed = static_cast< size_t >(detail::FallbackHashSeed);
      const size_t inner = getByteHash(data, size, seed ^ ipad);
      unsigned char innerBytes[sizeof(size_t)];
      for (size_t i = 0; i < sizeof(size_t); ++i)
      {
        innerBytes[i] = static_cast< unsigned char >(
            (inner >> (i * detail::ByteBits)) & detail::LowByteMask);
      }
      return getByteHash(innerBytes, sizeof(innerBytes), seed ^ opad);
    }
  };
}

#endif

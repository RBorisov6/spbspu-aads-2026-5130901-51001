#ifndef BORISOV_XXHASH_HPP
#define BORISOV_XXHASH_HPP

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>

namespace borisov
{
  namespace detail
  {
    static const uint32_t PRIME1 = 2654435761u;
    static const uint32_t PRIME2 = 2246822519u;
    static const uint32_t PRIME3 = 3266489917u;
    static const uint32_t PRIME4 =  668265263u;
    static const uint32_t PRIME5 =  374761393u;

    inline uint32_t rotl32(uint32_t x, int r)
    {
      return (x << r) | (x >> (32 - r));
    }
  }

  inline uint32_t xxhash32(const void* data, std::size_t len, uint32_t seed = 0)
  {
    using namespace detail;
    const uint8_t* p = static_cast<const uint8_t*>(data);
    const uint8_t* end = p + len;
    uint32_t h32 = 0;

    if (len >= 16)
    {
      const uint8_t* limit = end - 16;
      uint32_t v1 = seed + PRIME1 + PRIME2;
      uint32_t v2 = seed + PRIME2;
      uint32_t v3 = seed;
      uint32_t v4 = seed - PRIME1;

      do
      {
        uint32_t lane = 0;
        std::memcpy(&lane, p, 4);
        p += 4;
        v1 = PRIME1 * rotl32(v1 + PRIME2 * lane, 13);
        std::memcpy(&lane, p, 4);
        p += 4;
        v2 = PRIME1 * rotl32(v2 + PRIME2 * lane, 13);
        std::memcpy(&lane, p, 4);
        p += 4;
        v3 = PRIME1 * rotl32(v3 + PRIME2 * lane, 13);
        std::memcpy(&lane, p, 4);
        p += 4;
        v4 = PRIME1 * rotl32(v4 + PRIME2 * lane, 13);
      }
      while (p <= limit);

      h32 = rotl32(v1, 1) + rotl32(v2, 7) + rotl32(v3, 12) + rotl32(v4, 18);
    }
    else
    {
      h32 = seed + PRIME5;
    }

    h32 += static_cast<uint32_t>(len);

    while (p + 4 <= end)
    {
      uint32_t lane = 0;
      std::memcpy(&lane, p, 4);
        p += 4;
      h32 = PRIME1 * rotl32(h32 + PRIME3 * lane, 17);
    }

    while (p < end)
    {
      h32 = PRIME4 * rotl32(h32 + PRIME5 * static_cast<uint32_t>(*p), 11);
      ++p;
    }

    h32 ^= h32 >> 15;
    h32 *= PRIME2;
    h32 ^= h32 >> 13;
    h32 *= PRIME3;
    h32 ^= h32 >> 16;
    return h32;
  }

  struct XxHash32
  {
    std::size_t operator()(const std::string& key) const
    {
      return static_cast<std::size_t>(xxhash32(key.data(), key.size()));
    }

    template< class First, class Second >
    std::size_t operator()(const std::pair< First, Second >& p) const
    {
      uint32_t h1 = xxhash32(p.first.data(), p.first.size());
      uint32_t h2 = xxhash32(p.second.data(), p.second.size(), h1);
      return static_cast<std::size_t>(h2);
    }
  };
}

#endif

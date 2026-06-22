#ifndef BORISOV_HASHTABLE_HPP
#define BORISOV_HASHTABLE_HPP

#include "../common/list.hpp"
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <functional>

namespace borisov
{
  template< class Key, class Value, class Hash = std::hash< Key >, class Equal = std::equal_to< Key > >
  class HashTable
  {
  public:
    using Bucket = List< std::pair< Key, Value > >;

    explicit HashTable(std::size_t slots = 11);
    HashTable(const HashTable& other);
    HashTable(HashTable&& other) noexcept;
    ~HashTable();

    HashTable& operator=(const HashTable& other);
    HashTable& operator=(HashTable&& other) noexcept;

    // Throws std::overflow_error if size_ >= slots_ (table is full, call rehash first)
    void add(const Key& k, const Value& v);

    // Throws std::out_of_range if key not found
    Value drop(const Key& k);

    bool has(const Key& k) const;

    // Rebuild the table with a new slot count
    void rehash(std::size_t slots);

    Value& at(const Key& k);
    const Value& at(const Key& k) const;

    std::size_t size() const { return size_; }
    std::size_t slots() const { return slots_; }
    bool empty() const { return size_ == 0; }

    void clear();

    Bucket& bucket(std::size_t idx) { return buckets_[idx]; }
    const Bucket& bucket(std::size_t idx) const { return buckets_[idx]; }

  private:
    std::size_t slots_;
    std::size_t size_;
    Bucket* buckets_;

    Hash hasher_;
    Equal equal_;

    std::size_t bucketIndex(const Key& k) const
    {
      return hasher_(k) % slots_;
    }
  };

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable(std::size_t slots):
    slots_(slots == 0 ? 1 : slots),
    size_(0),
    buckets_(new Bucket[slots_ == 0 ? 1 : slots_])
  {}

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable(const HashTable& other):
    slots_(other.slots_),
    size_(other.size_),
    buckets_(new Bucket[other.slots_])
  {
    for (std::size_t i = 0; i < slots_; ++i)
    {
      buckets_[i] = other.buckets_[i];
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable(HashTable&& other) noexcept:
    slots_(other.slots_),
    size_(other.size_),
    buckets_(other.buckets_)
  {
    other.buckets_ = nullptr;
    other.size_ = 0;
    other.slots_ = 0;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::~HashTable()
  {
    delete[] buckets_;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >&
  HashTable< Key, Value, Hash, Equal >::operator=(const HashTable& other)
  {
    if (this != &other)
    {
      HashTable tmp(other);
      using std::swap;
      swap(slots_, tmp.slots_);
      swap(size_, tmp.size_);
      swap(buckets_, tmp.buckets_);
    }
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >&
  HashTable< Key, Value, Hash, Equal >::operator=(HashTable&& other) noexcept
  {
    if (this != &other)
    {
      delete[] buckets_;
      slots_ = other.slots_;
      size_ = other.size_;
      buckets_ = other.buckets_;
      other.buckets_ = nullptr;
      other.size_ = 0;
      other.slots_ = 0;
    }
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::add(const Key& k, const Value& v)
  {
    if (size_ >= slots_)
    {
      throw std::overflow_error("HashTable is full; call rehash to expand");
    }
    std::size_t idx = bucketIndex(k);
    buckets_[idx].pushBack(std::make_pair(k, v));
    ++size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  Value HashTable< Key, Value, Hash, Equal >::drop(const Key& k)
  {
    std::size_t idx = bucketIndex(k);
    Bucket& b = buckets_[idx];
    for (auto it = b.begin(); it != b.end(); ++it)
    {
      if (equal_(it->first, k))
      {
        Value v = it->second;
        b.erase(it);
        --size_;
        return v;
      }
    }
    throw std::out_of_range("HashTable::drop: key not found");
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::has(const Key& k) const
  {
    std::size_t idx = bucketIndex(k);
    const Bucket& b = buckets_[idx];
    for (auto it = b.begin(); it != b.end(); ++it)
    {
      if (equal_(it->first, k))
      {
        return true;
      }
    }
    return false;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::rehash(std::size_t slots)
  {
    if (slots == 0)
    {
      slots = 1;
    }
    Bucket* newBuckets = new Bucket[slots];
    for (std::size_t i = 0; i < slots_; ++i)
    {
      for (auto it = buckets_[i].begin(); it != buckets_[i].end(); ++it)
      {
        std::size_t idx = hasher_(it->first) % slots;
        newBuckets[idx].pushBack(*it);
      }
    }
    delete[] buckets_;
    buckets_ = newBuckets;
    slots_ = slots;
  }

  template< class Key, class Value, class Hash, class Equal >
  Value& HashTable< Key, Value, Hash, Equal >::at(const Key& k)
  {
    std::size_t idx = bucketIndex(k);
    Bucket& b = buckets_[idx];
    for (auto it = b.begin(); it != b.end(); ++it)
    {
      if (equal_(it->first, k))
      {
        return it->second;
      }
    }
    throw std::out_of_range("HashTable::at: key not found");
  }

  template< class Key, class Value, class Hash, class Equal >
  const Value& HashTable< Key, Value, Hash, Equal >::at(const Key& k) const
  {
    std::size_t idx = bucketIndex(k);
    const Bucket& b = buckets_[idx];
    for (auto it = b.begin(); it != b.end(); ++it)
    {
      if (equal_(it->first, k))
      {
        return it->second;
      }
    }
    throw std::out_of_range("HashTable::at: key not found");
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::clear()
  {
    for (std::size_t i = 0; i < slots_; ++i)
    {
      buckets_[i].clear();
    }
    size_ = 0;
  }
}

#endif

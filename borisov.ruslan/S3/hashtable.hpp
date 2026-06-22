#ifndef BORISOV_HASHTABLE_HPP
#define BORISOV_HASHTABLE_HPP

#include "../common/list.hpp"
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <functional>

namespace borisov
{
  template< class Key, class Value, class Hash, class Equal >
  class HTIter;

  template< class Key, class Value, class Hash, class Equal >
  class HTCIter;

  template< class Key, class Value, class Hash = std::hash< Key >, class Equal = std::equal_to< Key > >
  class HashTable
  {
  public:
    using Bucket = List< std::pair< Key, Value > >;
    using iterator = HTIter< Key, Value, Hash, Equal >;
    using const_iterator = HTCIter< Key, Value, Hash, Equal >;

    explicit HashTable(std::size_t slots = 11);
    HashTable(const HashTable& other);
    HashTable(HashTable&& other) noexcept;
    ~HashTable();

    HashTable& operator=(const HashTable& other);
    HashTable& operator=(HashTable&& other) noexcept;

    void add(const Key& k, const Value& v);

    Value drop(const Key& k);

    bool has(const Key& k) const;

    void rehash(std::size_t slots);

    Value& at(const Key& k);
    const Value& at(const Key& k) const;

    std::size_t size() const { return size_; }
    std::size_t slots() const { return slots_; }
    bool empty() const { return size_ == 0; }

    void clear();

    Bucket& bucket(std::size_t idx) { return buckets_[idx]; }
    const Bucket& bucket(std::size_t idx) const { return buckets_[idx]; }

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    friend class HTIter< Key, Value, Hash, Equal >;
    friend class HTCIter< Key, Value, Hash, Equal >;

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

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::iterator
  HashTable< Key, Value, Hash, Equal >::begin()
  {
    for (std::size_t i = 0; i < slots_; ++i)
    {
      if (!buckets_[i].empty())
      {
        return iterator(this, i, buckets_[i].begin());
      }
    }
    return end();
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::iterator
  HashTable< Key, Value, Hash, Equal >::end()
  {
    return iterator(this, slots_, typename Bucket::iterator());
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::const_iterator
  HashTable< Key, Value, Hash, Equal >::begin() const
  {
    for (std::size_t i = 0; i < slots_; ++i)
    {
      if (!buckets_[i].empty())
      {
        return const_iterator(this, i, buckets_[i].cbegin());
      }
    }
    return end();
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::const_iterator
  HashTable< Key, Value, Hash, Equal >::end() const
  {
    return const_iterator(this, slots_, typename Bucket::const_iterator());
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::const_iterator
  HashTable< Key, Value, Hash, Equal >::cbegin() const
  {
    return begin();
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::const_iterator
  HashTable< Key, Value, Hash, Equal >::cend() const
  {
    return end();
  }

  template< class Key, class Value, class Hash, class Equal >
  class HTIter
  {
  public:
    using HT = HashTable< Key, Value, Hash, Equal >;
    using BucketIter = typename HT::Bucket::iterator;
    using value_type = std::pair< Key, Value >;

    HTIter():
      table_(nullptr),
      bucketIdx_(0),
      it_()
    {}

    value_type& operator*() const { return *it_; }
    value_type* operator->() const { return &(*it_); }

    HTIter& operator++()
    {
      ++it_;
      advance();
      return *this;
    }

    HTIter operator++(int)
    {
      HTIter old(*this);
      ++(*this);
      return old;
    }

    bool operator==(const HTIter& o) const
    {
      return table_ == o.table_ && bucketIdx_ == o.bucketIdx_ && it_ == o.it_;
    }

    bool operator!=(const HTIter& o) const { return !(*this == o); }

  private:
    friend class HashTable< Key, Value, Hash, Equal >;

    HT* table_;
    std::size_t bucketIdx_;
    BucketIter it_;

    HTIter(HT* t, std::size_t idx, BucketIter it):
      table_(t),
      bucketIdx_(idx),
      it_(it)
    {}

    void advance()
    {
      while (bucketIdx_ < table_->slots_ && it_ == table_->buckets_[bucketIdx_].end())
      {
        ++bucketIdx_;
        if (bucketIdx_ < table_->slots_)
        {
          it_ = table_->buckets_[bucketIdx_].begin();
        }
        else
        {
          it_ = BucketIter();
        }
      }
    }
  };

  template< class Key, class Value, class Hash, class Equal >
  class HTCIter
  {
  public:
    using HT = HashTable< Key, Value, Hash, Equal >;
    using BucketCIter = typename HT::Bucket::const_iterator;
    using value_type = std::pair< Key, Value >;

    HTCIter():
      table_(nullptr),
      bucketIdx_(0),
      it_()
    {}

    const value_type& operator*() const { return *it_; }
    const value_type* operator->() const { return &(*it_); }

    HTCIter& operator++()
    {
      ++it_;
      advance();
      return *this;
    }

    HTCIter operator++(int)
    {
      HTCIter old(*this);
      ++(*this);
      return old;
    }

    bool operator==(const HTCIter& o) const
    {
      return table_ == o.table_ && bucketIdx_ == o.bucketIdx_ && it_ == o.it_;
    }

    bool operator!=(const HTCIter& o) const { return !(*this == o); }

  private:
    friend class HashTable< Key, Value, Hash, Equal >;

    const HT* table_;
    std::size_t bucketIdx_;
    BucketCIter it_;

    HTCIter(const HT* t, std::size_t idx, BucketCIter it):
      table_(t),
      bucketIdx_(idx),
      it_(it)
    {}

    void advance()
    {
      while (bucketIdx_ < table_->slots_ && it_ == table_->buckets_[bucketIdx_].cend())
      {
        ++bucketIdx_;
        if (bucketIdx_ < table_->slots_)
        {
          it_ = table_->buckets_[bucketIdx_].cbegin();
        }
        else
        {
          it_ = BucketCIter();
        }
      }
    }
  };
}

#endif

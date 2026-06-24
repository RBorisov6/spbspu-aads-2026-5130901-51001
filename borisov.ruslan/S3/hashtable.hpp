#ifndef BORISOV_HASHTABLE_HPP
#define BORISOV_HASHTABLE_HPP

#include "../common/list.hpp"
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>

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
    using ResizeFunc = std::function< std::size_t(std::size_t) >;

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

    std::size_t size() const;
    std::size_t slots() const;
    bool empty() const;

    double loadFactor() const;
    std::size_t longestChain() const;

    void setMaxLoadFactor(double limit);
    void setMaxChainLength(std::size_t limit);
    void setResizeFunc(ResizeFunc f);

    void clear();

    Bucket& bucket(std::size_t idx);
    const Bucket& bucket(std::size_t idx) const;

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
    double maxLoadFactor_;
    std::size_t maxChainLen_;
    ResizeFunc resizeFunc_;

    void swapWith(HashTable& other) noexcept;

    std::size_t bucketIndex(const Key& k) const;

    static std::size_t defaultResize(std::size_t n);
  };

  template< class Key, class Value, class Hash, class Equal >
  std::size_t HashTable< Key, Value, Hash, Equal >::defaultResize(std::size_t n)
  {
    return n < 10 ? 20 : n * 2 + 1;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable(std::size_t slots):
    slots_(slots == 0 ? 1 : slots),
    size_(0),
    buckets_(new Bucket[slots_ == 0 ? 1 : slots_]),
    hasher_(),
    equal_(),
    maxLoadFactor_(std::numeric_limits< double >::infinity()),
    maxChainLen_(std::numeric_limits< std::size_t >::max()),
    resizeFunc_(defaultResize)
  {}

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable(const HashTable& other):
    slots_(other.slots_),
    size_(other.size_),
    buckets_(new Bucket[other.slots_]),
    hasher_(other.hasher_),
    equal_(other.equal_),
    maxLoadFactor_(other.maxLoadFactor_),
    maxChainLen_(other.maxChainLen_),
    resizeFunc_(other.resizeFunc_)
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
    buckets_(other.buckets_),
    hasher_(std::move(other.hasher_)),
    equal_(std::move(other.equal_)),
    maxLoadFactor_(other.maxLoadFactor_),
    maxChainLen_(other.maxChainLen_),
    resizeFunc_(std::move(other.resizeFunc_))
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
      swapWith(tmp);
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
      hasher_ = std::move(other.hasher_);
      equal_ = std::move(other.equal_);
      maxLoadFactor_ = other.maxLoadFactor_;
      maxChainLen_ = other.maxChainLen_;
      resizeFunc_ = std::move(other.resizeFunc_);
      other.buckets_ = nullptr;
      other.size_ = 0;
      other.slots_ = 0;
    }
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::swapWith(HashTable& other) noexcept
  {
    using std::swap;
    swap(slots_, other.slots_);
    swap(size_, other.size_);
    swap(buckets_, other.buckets_);
    swap(hasher_, other.hasher_);
    swap(equal_, other.equal_);
    swap(maxLoadFactor_, other.maxLoadFactor_);
    swap(maxChainLen_, other.maxChainLen_);
    swap(resizeFunc_, other.resizeFunc_);
  }

  template< class Key, class Value, class Hash, class Equal >
  std::size_t HashTable< Key, Value, Hash, Equal >::bucketIndex(const Key& k) const
  {
    return hasher_(k) % slots_;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::add(const Key& k, const Value& v)
  {
    if (slots_ > 0)
    {
      const double newLf = static_cast< double >(size_ + 1) / static_cast< double >(slots_);
      if (newLf > maxLoadFactor_)
      {
        rehash(resizeFunc_(slots_));
      }
    }
    if (buckets_[bucketIndex(k)].size() >= maxChainLen_)
    {
      rehash(resizeFunc_(slots_));
    }
    buckets_[bucketIndex(k)].pushBack(std::make_pair(k, v));
    ++size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  Value HashTable< Key, Value, Hash, Equal >::drop(const Key& k)
  {
    const std::size_t idx = bucketIndex(k);
    Bucket& b = buckets_[idx];
    for (auto it = b.begin(); it != b.end(); ++it)
    {
      if (equal_(it->first, k))
      {
        const Value v = it->second;
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
    const std::size_t idx = bucketIndex(k);
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
        const std::size_t idx = hasher_(it->first) % slots;
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
    const std::size_t idx = bucketIndex(k);
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
    const std::size_t idx = bucketIndex(k);
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
  std::size_t HashTable< Key, Value, Hash, Equal >::size() const
  {
    return size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  std::size_t HashTable< Key, Value, Hash, Equal >::slots() const
  {
    return slots_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::empty() const
  {
    return size_ == 0;
  }

  template< class Key, class Value, class Hash, class Equal >
  double HashTable< Key, Value, Hash, Equal >::loadFactor() const
  {
    if (slots_ == 0)
    {
      return 0.0;
    }
    return static_cast< double >(size_) / static_cast< double >(slots_);
  }

  template< class Key, class Value, class Hash, class Equal >
  std::size_t HashTable< Key, Value, Hash, Equal >::longestChain() const
  {
    std::size_t longest = 0;
    for (std::size_t i = 0; i < slots_; ++i)
    {
      const std::size_t len = buckets_[i].size();
      if (len > longest)
      {
        longest = len;
      }
    }
    return longest;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::setMaxLoadFactor(double limit)
  {
    maxLoadFactor_ = limit;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::setMaxChainLength(std::size_t limit)
  {
    maxChainLen_ = limit;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::setResizeFunc(ResizeFunc f)
  {
    resizeFunc_ = f;
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
  typename HashTable< Key, Value, Hash, Equal >::Bucket&
  HashTable< Key, Value, Hash, Equal >::bucket(std::size_t idx)
  {
    return buckets_[idx];
  }

  template< class Key, class Value, class Hash, class Equal >
  const typename HashTable< Key, Value, Hash, Equal >::Bucket&
  HashTable< Key, Value, Hash, Equal >::bucket(std::size_t idx) const
  {
    return buckets_[idx];
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

    value_type& operator*() const
    {
      return *it_;
    }

    value_type* operator->() const
    {
      return &(*it_);
    }

    HTIter& operator++()
    {
      ++it_;
      advance();
      return *this;
    }

    HTIter operator++(int)
    {
      const HTIter old(*this);
      ++(*this);
      return old;
    }

    bool operator==(const HTIter& o) const
    {
      return table_ == o.table_ && bucketIdx_ == o.bucketIdx_ && it_ == o.it_;
    }

    bool operator!=(const HTIter& o) const
    {
      return !(*this == o);
    }

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

    const value_type& operator*() const
    {
      return *it_;
    }

    const value_type* operator->() const
    {
      return &(*it_);
    }

    HTCIter& operator++()
    {
      ++it_;
      advance();
      return *this;
    }

    HTCIter operator++(int)
    {
      const HTCIter old(*this);
      ++(*this);
      return old;
    }

    bool operator==(const HTCIter& o) const
    {
      return table_ == o.table_ && bucketIdx_ == o.bucketIdx_ && it_ == o.it_;
    }

    bool operator!=(const HTCIter& o) const
    {
      return !(*this == o);
    }

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

#ifndef BORISOV_TREESET_HPP
#define BORISOV_TREESET_HPP

#include "avltree.hpp"
#include <cstddef>
#include <functional>

namespace borisov
{
  template< class Key, class Compare = std::less< Key > >
  class TreeSet
  {
    using Tree = AVLTree< Key, bool, Compare >;

  public:
    class iterator
    {
    public:
      iterator()
      {}

      explicit iterator(typename Tree::const_iterator it):
        it_(it)
      {}

      explicit iterator(typename Tree::iterator it):
        it_(it)
      {}

      const Key& operator*() const
      {
        return it_->first;
      }

      const Key* operator->() const
      {
        return &(it_->first);
      }

      iterator& operator++()
      {
        ++it_;
        return *this;
      }

      iterator operator++(int)
      {
        iterator old(*this);
        ++(*this);
        return old;
      }

      bool operator==(const iterator& o) const
      {
        return it_ == o.it_;
      }

      bool operator!=(const iterator& o) const
      {
        return !(*this == o);
      }

    private:
      typename Tree::const_iterator it_;
    };

    using const_iterator = iterator;

    void insert(const Key& k)
    {
      tree_.push(k, true);
    }

    void erase(const Key& k)
    {
      tree_.drop(k);
    }

    bool count(const Key& k) const
    {
      return tree_.has(k);
    }

    iterator find(const Key& k) const
    {
      return iterator(tree_.find(k));
    }

    std::size_t size() const
    {
      return tree_.size();
    }

    bool empty() const
    {
      return tree_.empty();
    }

    void clear()
    {
      tree_.clear();
    }

    iterator begin() const
    {
      return iterator(tree_.cbegin());
    }

    iterator end() const
    {
      return iterator(tree_.cend());
    }

    const_iterator cbegin() const
    {
      return begin();
    }

    const_iterator cend() const
    {
      return end();
    }

  private:
    Tree tree_;
  };
}

#endif

#ifndef BORISOV_TREEMAP_HPP
#define BORISOV_TREEMAP_HPP

#include "avltree.hpp"
#include <cstddef>
#include <functional>

namespace borisov
{
  template< class Key, class Value, class Compare = std::less< Key > >
  class TreeMap
  {
    using Tree = AVLTree< Key, Value, Compare >;

  public:
    using iterator = typename Tree::iterator;
    using const_iterator = typename Tree::const_iterator;

    TreeMap() {}

    void insert(const Key& k, const Value& v)
    {
      tree_.push(k, v);
    }

    void erase(const Key& k)
    {
      tree_.drop(k);
    }

    Value& operator[](const Key& k)
    {
      if (!tree_.has(k))
      {
        tree_.push(k, Value());
      }
      return tree_.get(k);
    }

    Value& at(const Key& k)
    {
      return tree_.get(k);
    }

    const Value& at(const Key& k) const
    {
      return tree_.get(k);
    }

    bool count(const Key& k) const
    {
      return tree_.has(k);
    }

    iterator find(const Key& k) { return tree_.find(k); }
    const_iterator find(const Key& k) const { return tree_.find(k); }

    std::size_t size() const { return tree_.size(); }
    bool empty() const { return tree_.empty(); }
    void clear() { tree_.clear(); }

    iterator begin() { return tree_.begin(); }
    iterator end() { return tree_.end(); }
    const_iterator begin() const { return tree_.begin(); }
    const_iterator end() const { return tree_.end(); }
    const_iterator cbegin() const { return tree_.cbegin(); }
    const_iterator cend() const { return tree_.cend(); }

  private:
    Tree tree_;
  };
}

#endif

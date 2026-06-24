#ifndef BORISOV_BSTREE_HPP
#define BORISOV_BSTREE_HPP

#include <cstddef>
#include <stdexcept>
#include <functional>
#include <utility>

namespace borisov
{
  namespace detail
  {
    template< class Key, class Value >
    struct BSTNode
    {
      std::pair< Key, Value > data_;
      BSTNode* left_;
      BSTNode* right_;
      BSTNode* parent_;

      BSTNode(const Key& k, const Value& v, BSTNode* p):
        data_(k, v),
        left_(nullptr),
        right_(nullptr),
        parent_(p)
      {}
    };
  }

  template< class Key, class Value >
  class BSTIterator;

  template< class Key, class Value >
  class BSTConstIterator;

  template< class Key, class Value, class Compare = std::less< Key > >
  class BSTree
  {
  public:
    using Node = detail::BSTNode< Key, Value >;
    using iterator = BSTIterator< Key, Value >;
    using const_iterator = BSTConstIterator< Key, Value >;

    BSTree():
      root_(nullptr),
      size_(0),
      comp_()
    {}

    BSTree(const BSTree& other):
      root_(nullptr),
      size_(0),
      comp_(other.comp_)
    {
      for (auto it = other.cbegin(); it != other.cend(); ++it)
      {
        push(it->first, it->second);
      }
    }

    BSTree(BSTree&& other) noexcept:
      root_(other.root_),
      size_(other.size_),
      comp_(std::move(other.comp_))
    {
      other.root_ = nullptr;
      other.size_ = 0;
    }

    ~BSTree()
    {
      clear();
    }

    BSTree& operator=(const BSTree& other)
    {
      if (this != &other)
      {
        BSTree tmp(other);
        swapWith(tmp);
      }
      return *this;
    }

    BSTree& operator=(BSTree&& other) noexcept
    {
      if (this != &other)
      {
        clear();
        root_ = other.root_;
        size_ = other.size_;
        comp_ = std::move(other.comp_);
        other.root_ = nullptr;
        other.size_ = 0;
      }
      return *this;
    }

    void push(const Key& k, const Value& v)
    {
      if (!root_)
      {
        root_ = new Node(k, v, nullptr);
        ++size_;
        return;
      }
      Node* curr = root_;
      while (true)
      {
        if (comp_(k, curr->data_.first))
        {
          if (!curr->left_)
          {
            curr->left_ = new Node(k, v, curr);
            ++size_;
            return;
          }
          curr = curr->left_;
        }
        else if (comp_(curr->data_.first, k))
        {
          if (!curr->right_)
          {
            curr->right_ = new Node(k, v, curr);
            ++size_;
            return;
          }
          curr = curr->right_;
        }
        else
        {
          curr->data_.second = v;
          return;
        }
      }
    }

    Value& get(const Key& k)
    {
      return findOrThrow(k)->data_.second;
    }

    const Value& get(const Key& k) const
    {
      return findOrThrowConst(k)->data_.second;
    }

    Value drop(const Key& k)
    {
      Node* n = findOrThrow(k);
      Value val = n->data_.second;
      removeNode(n);
      return val;
    }

    bool has(const Key& k) const
    {
      return findOrNullConst(k) != nullptr;
    }

    iterator find(const Key& k);
    const_iterator find(const Key& k) const;

    void clear()
    {
      freeTree(root_);
      root_ = nullptr;
      size_ = 0;
    }

    bool empty() const
    {
      return size_ == 0;
    }

    std::size_t size() const
    {
      return size_;
    }

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const
    {
      return begin();
    }

    const_iterator cend() const
    {
      return end();
    }

    const_iterator rotateLeft(const_iterator it)
    {
      Node* n = const_cast< Node* >(it.node_);
      Node* p = n->parent_;
      p->right_ = n->left_;
      if (n->left_)
      {
        n->left_->parent_ = p;
      }
      n->left_ = p;
      n->parent_ = p->parent_;
      if (p->parent_)
      {
        if (p->parent_->left_ == p)
        {
          p->parent_->left_ = n;
        }
        else
        {
          p->parent_->right_ = n;
        }
      }
      else
      {
        root_ = n;
      }
      p->parent_ = n;
      return const_iterator(n);
    }

    const_iterator rotateRight(const_iterator it)
    {
      Node* n = const_cast< Node* >(it.node_);
      Node* p = n->parent_;
      p->left_ = n->right_;
      if (n->right_)
      {
        n->right_->parent_ = p;
      }
      n->right_ = p;
      n->parent_ = p->parent_;
      if (p->parent_)
      {
        if (p->parent_->left_ == p)
        {
          p->parent_->left_ = n;
        }
        else
        {
          p->parent_->right_ = n;
        }
      }
      else
      {
        root_ = n;
      }
      p->parent_ = n;
      return const_iterator(n);
    }

    const_iterator rotateLargeLeft(const_iterator it)
    {
      rotateRight(it);
      return rotateLeft(it);
    }

    const_iterator rotateLargeRight(const_iterator it)
    {
      rotateLeft(it);
      return rotateRight(it);
    }

    std::size_t height() const
    {
      return nodeHeight(root_);
    }

    std::size_t height(const_iterator it) const
    {
      return nodeHeight(it.node_);
    }

  private:
    Node* root_;
    std::size_t size_;
    Compare comp_;

    void swapWith(BSTree& other) noexcept
    {
      using std::swap;
      swap(root_, other.root_);
      swap(size_, other.size_);
      swap(comp_, other.comp_);
    }

    static void freeTree(Node* n)
    {
      if (!n)
      {
        return;
      }
      freeTree(n->left_);
      freeTree(n->right_);
      delete n;
    }

    Node* findOrNull(const Key& k) const
    {
      return const_cast< Node* >(findOrNullConst(k));
    }

    const Node* findOrNullConst(const Key& k) const
    {
      const Node* curr = root_;
      while (curr)
      {
        if (comp_(k, curr->data_.first))
        {
          curr = curr->left_;
        }
        else if (comp_(curr->data_.first, k))
        {
          curr = curr->right_;
        }
        else
        {
          return curr;
        }
      }
      return nullptr;
    }

    Node* findOrThrow(const Key& k)
    {
      Node* n = findOrNull(k);
      if (!n)
      {
        throw std::out_of_range("BSTree: key not found");
      }
      return n;
    }

    const Node* findOrThrowConst(const Key& k) const
    {
      const Node* n = findOrNullConst(k);
      if (!n)
      {
        throw std::out_of_range("BSTree: key not found");
      }
      return n;
    }

    void removeNode(Node* node)
    {
      if (node->left_ && node->right_)
      {
        Node* succ = node->right_;
        while (succ->left_)
        {
          succ = succ->left_;
        }
        node->data_ = succ->data_;
        removeNode(succ);
        return;
      }
      Node* child = node->left_ ? node->left_ : node->right_;
      if (child)
      {
        child->parent_ = node->parent_;
      }
      if (!node->parent_)
      {
        root_ = child;
      }
      else if (node->parent_->left_ == node)
      {
        node->parent_->left_ = child;
      }
      else
      {
        node->parent_->right_ = child;
      }
      delete node;
      --size_;
    }

    static std::size_t nodeHeight(const Node* n)
    {
      if (!n)
      {
        return 0;
      }
      const std::size_t lh = nodeHeight(n->left_);
      const std::size_t rh = nodeHeight(n->right_);
      return 1 + (lh > rh ? lh : rh);
    }
  };

  template< class Key, class Value >
  class BSTIterator
  {
  public:
    using Node = detail::BSTNode< Key, Value >;
    using value_type = std::pair< Key, Value >;

    BSTIterator():
      node_(nullptr)
    {}

    value_type& operator*() const
    {
      return node_->data_;
    }

    value_type* operator->() const
    {
      return &(node_->data_);
    }

    BSTIterator& operator++()
    {
      if (node_->right_)
      {
        node_ = node_->right_;
        while (node_->left_)
        {
          node_ = node_->left_;
        }
      }
      else
      {
        Node* par = node_->parent_;
        while (par && node_ == par->right_)
        {
          node_ = par;
          par = par->parent_;
        }
        node_ = par;
      }
      return *this;
    }

    BSTIterator operator++(int)
    {
      BSTIterator old(*this);
      ++(*this);
      return old;
    }

    bool operator==(const BSTIterator& o) const
    {
      return node_ == o.node_;
    }

    bool operator!=(const BSTIterator& o) const
    {
      return !(*this == o);
    }

  private:
    template< class K, class V, class C >
    friend class BSTree;

    template< class K, class V >
    friend class BSTConstIterator;

    Node* node_;

    explicit BSTIterator(Node* n):
      node_(n)
    {}
  };

  template< class Key, class Value >
  class BSTConstIterator
  {
  public:
    using Node = detail::BSTNode< Key, Value >;
    using value_type = std::pair< Key, Value >;

    BSTConstIterator():
      node_(nullptr)
    {}

    BSTConstIterator(const BSTIterator< Key, Value >& it):
      node_(it.node_)
    {}

    const value_type& operator*() const
    {
      return node_->data_;
    }

    const value_type* operator->() const
    {
      return &(node_->data_);
    }

    BSTConstIterator& operator++()
    {
      if (node_->right_)
      {
        node_ = node_->right_;
        while (node_->left_)
        {
          node_ = node_->left_;
        }
      }
      else
      {
        const Node* par = node_->parent_;
        while (par && node_ == par->right_)
        {
          node_ = par;
          par = par->parent_;
        }
        node_ = par;
      }
      return *this;
    }

    BSTConstIterator operator++(int)
    {
      BSTConstIterator old(*this);
      ++(*this);
      return old;
    }

    bool operator==(const BSTConstIterator& o) const
    {
      return node_ == o.node_;
    }

    bool operator!=(const BSTConstIterator& o) const
    {
      return !(*this == o);
    }

  private:
    template< class K, class V, class C >
    friend class BSTree;

    const Node* node_;

    explicit BSTConstIterator(const Node* n):
      node_(n)
    {}
  };

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator
  BSTree< Key, Value, Compare >::begin()
  {
    Node* n = root_;
    if (n)
    {
      while (n->left_)
      {
        n = n->left_;
      }
    }
    return iterator(n);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator
  BSTree< Key, Value, Compare >::end()
  {
    return iterator(nullptr);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::begin() const
  {
    const Node* n = root_;
    if (n)
    {
      while (n->left_)
      {
        n = n->left_;
      }
    }
    return const_iterator(n);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::end() const
  {
    return const_iterator(nullptr);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator
  BSTree< Key, Value, Compare >::find(const Key& k)
  {
    return iterator(findOrNull(k));
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
  BSTree< Key, Value, Compare >::find(const Key& k) const
  {
    return const_iterator(findOrNullConst(k));
  }
}

#endif

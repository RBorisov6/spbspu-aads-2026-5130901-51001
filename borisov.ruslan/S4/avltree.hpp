#ifndef BORISOV_AVLTREE_HPP
#define BORISOV_AVLTREE_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>

namespace borisov
{
  namespace detail
  {
    template< class Key, class Value >
    struct AVLNode
    {
      std::pair< Key, Value > data_;
      AVLNode* left_;
      AVLNode* right_;
      AVLNode* parent_;
      int height_;

      AVLNode(const Key& k, const Value& v, AVLNode* p):
        data_(k, v),
        left_(nullptr),
        right_(nullptr),
        parent_(p),
        height_(1)
      {}
    };
  }

  template< class Key, class Value >
  class AVLIterator;

  template< class Key, class Value >
  class AVLConstIterator;

  template< class Key, class Value, class Compare = std::less< Key > >
  class AVLTree
  {
  public:
    using Node = detail::AVLNode< Key, Value >;
    using iterator = AVLIterator< Key, Value >;
    using const_iterator = AVLConstIterator< Key, Value >;

    AVLTree():
      root_(nullptr),
      size_(0),
      comp_()
    {}

    AVLTree(const AVLTree& other):
      root_(nullptr),
      size_(0),
      comp_(other.comp_)
    {
      for (auto it = other.cbegin(); it != other.cend(); ++it)
      {
        push(it->first, it->second);
      }
    }

    AVLTree(AVLTree&& other) noexcept:
      root_(other.root_),
      size_(other.size_),
      comp_(std::move(other.comp_))
    {
      other.root_ = nullptr;
      other.size_ = 0;
    }

    ~AVLTree()
    {
      clear();
    }

    AVLTree& operator=(const AVLTree& other)
    {
      if (this != &other)
      {
        AVLTree tmp(other);
        swapWith(tmp);
      }
      return *this;
    }

    AVLTree& operator=(AVLTree&& other) noexcept
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
      root_ = insertNode(root_, nullptr, k, v);
    }

    Value& get(const Key& k)
    {
      Node* n = findOrNull(k);
      if (!n)
      {
        throw std::out_of_range("AVLTree: key not found");
      }
      return n->data_.second;
    }

    const Value& get(const Key& k) const
    {
      const Node* n = findOrNullConst(k);
      if (!n)
      {
        throw std::out_of_range("AVLTree: key not found");
      }
      return n->data_.second;
    }

    Value drop(const Key& k)
    {
      Node* n = findOrNull(k);
      if (!n)
      {
        throw std::out_of_range("AVLTree: key not found");
      }
      const Value val = n->data_.second;
      root_ = removeNode(root_, k, nullptr);
      return val;
    }

    bool has(const Key& k) const
    {
      return findOrNullConst(k) != nullptr;
    }

    void clear()
    {
      freeTree(root_);
      root_ = nullptr;
      size_ = 0;
    }

    bool empty() const { return size_ == 0; }
    std::size_t size() const { return size_; }

    iterator begin()
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

    iterator end() { return iterator(nullptr); }

    const_iterator begin() const
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

    const_iterator end() const { return const_iterator(nullptr); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    iterator find(const Key& k) { return iterator(findOrNull(k)); }
    const_iterator find(const Key& k) const { return const_iterator(findOrNullConst(k)); }

  private:
    Node* root_;
    std::size_t size_;
    Compare comp_;

    void swapWith(AVLTree& other) noexcept
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

    static int nodeHeight(const Node* n) { return n ? n->height_ : 0; }

    static void updateHeight(Node* n)
    {
      if (n)
      {
        const int lh = nodeHeight(n->left_);
        const int rh = nodeHeight(n->right_);
        n->height_ = 1 + (lh > rh ? lh : rh);
      }
    }

    static int balanceFactor(const Node* n)
    {
      return n ? nodeHeight(n->right_) - nodeHeight(n->left_) : 0;
    }

    Node* rotateLeft(Node* p)
    {
      Node* n = p->right_;
      p->right_ = n->left_;
      if (n->left_)
      {
        n->left_->parent_ = p;
      }
      n->left_ = p;
      p->parent_ = n;
      updateHeight(p);
      updateHeight(n);
      return n;
    }

    Node* rotateRight(Node* p)
    {
      Node* n = p->left_;
      p->left_ = n->right_;
      if (n->right_)
      {
        n->right_->parent_ = p;
      }
      n->right_ = p;
      p->parent_ = n;
      updateHeight(p);
      updateHeight(n);
      return n;
    }

    Node* balance(Node* n)
    {
      updateHeight(n);
      const int bf = balanceFactor(n);
      if (bf == 2)
      {
        if (balanceFactor(n->right_) < 0)
        {
          n->right_ = rotateRight(n->right_);
          n->right_->parent_ = n;
        }
        return rotateLeft(n);
      }
      if (bf == -2)
      {
        if (balanceFactor(n->left_) > 0)
        {
          n->left_ = rotateLeft(n->left_);
          n->left_->parent_ = n;
        }
        return rotateRight(n);
      }
      return n;
    }

    Node* insertNode(Node* n, Node* parent, const Key& k, const Value& v)
    {
      if (!n)
      {
        ++size_;
        return new Node(k, v, parent);
      }
      if (comp_(k, n->data_.first))
      {
        n->left_ = insertNode(n->left_, n, k, v);
        n->left_->parent_ = n;
      }
      else if (comp_(n->data_.first, k))
      {
        n->right_ = insertNode(n->right_, n, k, v);
        n->right_->parent_ = n;
      }
      else
      {
        n->data_.second = v;
        return n;
      }
      Node* result = balance(n);
      result->parent_ = parent;
      return result;
    }

    Node* removeNode(Node* n, const Key& k, Node* parent)
    {
      if (!n)
      {
        return nullptr;
      }
      if (comp_(k, n->data_.first))
      {
        n->left_ = removeNode(n->left_, k, n);
        if (n->left_)
        {
          n->left_->parent_ = n;
        }
      }
      else if (comp_(n->data_.first, k))
      {
        n->right_ = removeNode(n->right_, k, n);
        if (n->right_)
        {
          n->right_->parent_ = n;
        }
      }
      else if (!n->left_ || !n->right_)
      {
        Node* child = n->left_ ? n->left_ : n->right_;
        delete n;
        --size_;
        if (child)
        {
          child->parent_ = parent;
        }
        return child;
      }
      else
      {
        Node* succ = n->right_;
        while (succ->left_)
        {
          succ = succ->left_;
        }
        const Key succKey = succ->data_.first;
        n->data_ = succ->data_;
        n->right_ = removeNode(n->right_, succKey, n);
        if (n->right_)
        {
          n->right_->parent_ = n;
        }
      }
      Node* result = balance(n);
      result->parent_ = parent;
      return result;
    }
  };

  template< class Key, class Value >
  class AVLIterator
  {
  public:
    using Node = detail::AVLNode< Key, Value >;
    using value_type = std::pair< Key, Value >;

    AVLIterator(): node_(nullptr) {}

    value_type& operator*() const { return node_->data_; }
    value_type* operator->() const { return &(node_->data_); }

    AVLIterator& operator++()
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

    AVLIterator operator++(int)
    {
      AVLIterator old(*this);
      ++(*this);
      return old;
    }

    bool operator==(const AVLIterator& o) const { return node_ == o.node_; }
    bool operator!=(const AVLIterator& o) const { return !(*this == o); }

  private:
    template< class K, class V, class C >
    friend class AVLTree;

    template< class K, class V >
    friend class AVLConstIterator;

    Node* node_;

    explicit AVLIterator(Node* n): node_(n) {}
  };

  template< class Key, class Value >
  class AVLConstIterator
  {
  public:
    using Node = detail::AVLNode< Key, Value >;
    using value_type = std::pair< Key, Value >;

    AVLConstIterator(): node_(nullptr) {}

    AVLConstIterator(const AVLIterator< Key, Value >& it): node_(it.node_) {}

    const value_type& operator*() const { return node_->data_; }
    const value_type* operator->() const { return &(node_->data_); }

    AVLConstIterator& operator++()
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

    AVLConstIterator operator++(int)
    {
      AVLConstIterator old(*this);
      ++(*this);
      return old;
    }

    bool operator==(const AVLConstIterator& o) const { return node_ == o.node_; }
    bool operator!=(const AVLConstIterator& o) const { return !(*this == o); }

  private:
    template< class K, class V, class C >
    friend class AVLTree;

    const Node* node_;

    explicit AVLConstIterator(const Node* n): node_(n) {}
  };
}

#endif

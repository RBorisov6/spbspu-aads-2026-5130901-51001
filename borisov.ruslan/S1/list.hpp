#ifndef BORISOV_LIST_HPP
#define BORISOV_LIST_HPP

#include <cstddef>
#include <functional>
#include <utility>
#include <stdexcept>

namespace borisov
{
  template <class T> class LIter;
  template <class T> class LCIter;

  template <class T>
  class List
  {
  public:
    using iterator = LIter<T>;
    using const_iterator = LCIter<T>;

    List();
    List(const List& other);
    List(List&& other) noexcept;
    ~List();

    List& operator=(const List& other);
    List& operator=(List&& other) noexcept;

    void pushFront(const T& value);
    void pushBack(const T& value);
    void popFront();
    void popBack();

    bool empty() const;
    std::size_t size() const;
    void clear();

    T& front();
    const T& front() const;
    T& back();
    const T& back() const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    iterator insert(iterator pos, const T& value);
    iterator erase(iterator pos);

    void swap(List& other) noexcept;

    void splice(iterator pos, List& other) noexcept;
    void splice(iterator pos, List& other, iterator it) noexcept;
    void splice(iterator pos, List& other, iterator first, iterator last) noexcept;

    void sort() noexcept;

    template< class Compare >
    void sort(Compare cmp) noexcept;

    void merge(List& other) noexcept;

    template< class Compare >
    void merge(List& other, Compare cmp) noexcept;

    template< class Pred >
    iterator partition(Pred pred) noexcept;

  private:
    struct Node
    {
      T data_;
      Node* prev_;
      Node* next_;

      explicit Node(const T& val):
        data_(val),
        prev_(nullptr),
        next_(nullptr)
      {}
    };

    Node* head_;
    Node* tail_;
    std::size_t size_;

    void unlinkNode(Node* n) noexcept;
    void linkBefore(iterator pos, Node* n) noexcept;

    friend class LIter<T>;
    friend class LCIter<T>;
  };

  template <class T>
  class LIter
  {
    friend class List<T>;

  public:
    LIter();
    LIter(const LIter& other);
    LIter& operator=(const LIter& other);

    T& operator*() const;
    T* operator->() const;

    LIter& operator++();
    LIter operator++(int);

    LIter& operator--();
    LIter operator--(int);

    bool operator==(const LIter& other) const;
    bool operator!=(const LIter& other) const;

  private:
    using Node = typename List<T>::Node;
    Node* node_;

    explicit LIter(Node* node);
  };

  template <class T>
  class LCIter
  {
    friend class List<T>;

  public:
    LCIter();
    LCIter(const LCIter& other);
    LCIter& operator=(const LCIter& other);

    const T& operator*() const;
    const T* operator->() const;

    LCIter& operator++();
    LCIter operator++(int);

    LCIter& operator--();
    LCIter operator--(int);

    bool operator==(const LCIter& other) const;
    bool operator!=(const LCIter& other) const;

  private:
    using Node = typename List<T>::Node;
    const Node* node_;

    explicit LCIter(const Node* node);
  };

  template <class T>
  List<T>::List():
    head_(nullptr),
    tail_(nullptr),
    size_(0)
  {}

  template <class T>
  List<T>::List(const List& other):
    head_(nullptr),
    tail_(nullptr),
    size_(0)
  {
    for (const_iterator it = other.begin(); it != other.end(); ++it)
    {
      pushBack(*it);
    }
  }

  template <class T>
  List<T>::List(List&& other) noexcept:
    head_(other.head_),
    tail_(other.tail_),
    size_(other.size_)
  {
    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.size_ = 0;
  }

  template <class T>
  List<T>::~List()
  {
    clear();
  }

  template <class T>
  List<T>& List<T>::operator=(const List& other)
  {
    if (this != &other)
    {
      List tmp(other);
      swap(tmp);
    }
    return *this;
  }

  template <class T>
  List<T>& List<T>::operator=(List&& other) noexcept
  {
    if (this != &other)
    {
      clear();
      head_ = other.head_;
      tail_ = other.tail_;
      size_ = other.size_;
      other.head_ = nullptr;
      other.tail_ = nullptr;
      other.size_ = 0;
    }
    return *this;
  }

  template <class T>
  void List<T>::pushFront(const T& value)
  {
    Node* node = new Node(value);
    if (empty())
    {
      head_ = tail_ = node;
    }
    else
    {
      node->next_ = head_;
      head_->prev_ = node;
      head_ = node;
    }
    ++size_;
  }

  template <class T>
  void List<T>::pushBack(const T& value)
  {
    Node* node = new Node(value);
    if (empty())
    {
      head_ = tail_ = node;
    }
    else
    {
      node->prev_ = tail_;
      tail_->next_ = node;
      tail_ = node;
    }
    ++size_;
  }

  template <class T>
  void List<T>::popFront()
  {
    if (empty())
    {
      throw std::logic_error("popFront on empty list");
    }
    Node* oldHead = head_;
    head_ = head_->next_;
    if (head_ != nullptr)
    {
      head_->prev_ = nullptr;
    }
    else
    {
      tail_ = nullptr;
    }
    delete oldHead;
    --size_;
  }

  template <class T>
  void List<T>::popBack()
  {
    if (empty())
    {
      throw std::logic_error("popBack on empty list");
    }
    Node* oldTail = tail_;
    tail_ = tail_->prev_;
    if (tail_ != nullptr)
    {
      tail_->next_ = nullptr;
    }
    else
    {
      head_ = nullptr;
    }
    delete oldTail;
    --size_;
  }

  template <class T>
  bool List<T>::empty() const
  {
    return size_ == 0;
  }

  template <class T>
  std::size_t List<T>::size() const
  {
    return size_;
  }

  template <class T>
  void List<T>::clear()
  {
    while (!empty())
    {
      popFront();
    }
  }

  template <class T>
  T& List<T>::front()
  {
    if (empty())
    {
      throw std::logic_error("front on empty list");
    }
    return head_->data_;
  }

  template <class T>
  const T& List<T>::front() const
  {
    if (empty())
    {
      throw std::logic_error("front on empty list");
    }
    return head_->data_;
  }

  template <class T>
  T& List<T>::back()
  {
    if (empty())
    {
      throw std::logic_error("back on empty list");
    }
    return tail_->data_;
  }

  template <class T>
  const T& List<T>::back() const
  {
    if (empty())
    {
      throw std::logic_error("back on empty list");
    }
    return tail_->data_;
  }

  template <class T>
  typename List<T>::iterator List<T>::begin()
  {
    return iterator(head_);
  }

  template <class T>
  typename List<T>::iterator List<T>::end()
  {
    return iterator(nullptr);
  }

  template <class T>
  typename List<T>::const_iterator List<T>::begin() const
  {
    return const_iterator(head_);
  }

  template <class T>
  typename List<T>::const_iterator List<T>::end() const
  {
    return const_iterator(nullptr);
  }

  template <class T>
  typename List<T>::const_iterator List<T>::cbegin() const
  {
    return const_iterator(head_);
  }

  template <class T>
  typename List<T>::const_iterator List<T>::cend() const
  {
    return const_iterator(nullptr);
  }

  template <class T>
  typename List<T>::iterator List<T>::insert(iterator pos, const T& value)
  {
    if (pos == begin())
    {
      pushFront(value);
      return begin();
    }
    if (pos == end())
    {
      pushBack(value);
      iterator it = end();
      --it;
      return it;
    }
    Node* curr = pos.node_;
    Node* prev = curr->prev_;
    Node* node = new Node(value);
    node->prev_ = prev;
    node->next_ = curr;
    prev->next_ = node;
    curr->prev_ = node;
    ++size_;
    return iterator(node);
  }

  template <class T>
  typename List<T>::iterator List<T>::erase(iterator pos)
  {
    if (pos == end())
    {
      return end();
    }
    Node* curr = pos.node_;
    iterator nextIt = pos;
    ++nextIt;
    if (curr == head_)
    {
      popFront();
      return begin();
    }
    if (curr == tail_)
    {
      popBack();
      return end();
    }
    curr->prev_->next_ = curr->next_;
    curr->next_->prev_ = curr->prev_;
    delete curr;
    --size_;
    return nextIt;
  }

  template <class T>
  void List<T>::swap(List& other) noexcept
  {
    using std::swap;
    swap(head_, other.head_);
    swap(tail_, other.tail_);
    swap(size_, other.size_);
  }

  template <class T>
  void List<T>::unlinkNode(Node* n) noexcept
  {
    if (n->prev_ != nullptr)
    {
      n->prev_->next_ = n->next_;
    }
    else
    {
      head_ = n->next_;
    }
    if (n->next_ != nullptr)
    {
      n->next_->prev_ = n->prev_;
    }
    else
    {
      tail_ = n->prev_;
    }
    n->prev_ = nullptr;
    n->next_ = nullptr;
    --size_;
  }

  template <class T>
  void List<T>::linkBefore(iterator pos, Node* n) noexcept
  {
    Node* const curr = pos.node_;
    Node* const prev = (curr != nullptr) ? curr->prev_ : tail_;
    n->prev_ = prev;
    n->next_ = curr;
    if (prev != nullptr)
    {
      prev->next_ = n;
    }
    else
    {
      head_ = n;
    }
    if (curr != nullptr)
    {
      curr->prev_ = n;
    }
    else
    {
      tail_ = n;
    }
    ++size_;
  }

  template <class T>
  void List<T>::splice(iterator pos, List& other, iterator it) noexcept
  {
    if (it == other.end())
    {
      return;
    }
    other.unlinkNode(it.node_);
    linkBefore(pos, it.node_);
  }

  template <class T>
  void List<T>::splice(iterator pos, List& other, iterator first, iterator last) noexcept
  {
    iterator it = first;
    while (it != last)
    {
      iterator next = it;
      ++next;
      splice(pos, other, it);
      it = next;
    }
  }

  template <class T>
  void List<T>::splice(iterator pos, List& other) noexcept
  {
    splice(pos, other, other.begin(), other.end());
  }

  template <class T>
  template< class Compare >
  void List<T>::merge(List& other, Compare cmp) noexcept
  {
    iterator it = begin();
    iterator oit = other.begin();
    while (it != end() && oit != other.end())
    {
      if (cmp(*oit, *it))
      {
        iterator next = oit;
        ++next;
        splice(it, other, oit);
        oit = next;
      }
      else
      {
        ++it;
      }
    }
    splice(end(), other);
  }

  template <class T>
  void List<T>::merge(List& other) noexcept
  {
    merge(other, std::less< T >());
  }

  template <class T>
  template< class Compare >
  void List<T>::sort(Compare cmp) noexcept
  {
    if (size_ <= 1)
    {
      return;
    }
    List half;
    iterator mid = begin();
    const std::size_t halfSize = size_ / 2;
    for (std::size_t i = 0; i < halfSize; ++i)
    {
      ++mid;
    }
    half.splice(half.end(), *this, mid, end());
    sort(cmp);
    half.sort(cmp);
    merge(half, cmp);
  }

  template <class T>
  void List<T>::sort() noexcept
  {
    sort(std::less< T >());
  }

  template <class T>
  template< class Pred >
  typename List<T>::iterator List<T>::partition(Pred pred) noexcept
  {
    List trueList;
    List falseList;
    while (!empty())
    {
      iterator it = begin();
      if (pred(*it))
      {
        trueList.splice(trueList.end(), *this, it);
      }
      else
      {
        falseList.splice(falseList.end(), *this, it);
      }
    }
    const iterator result = falseList.begin();
    trueList.splice(trueList.end(), falseList);
    swap(trueList);
    return result;
  }

  template <class T>
  LIter<T>::LIter():
    node_(nullptr)
  {}

  template <class T>
  LIter<T>::LIter(const LIter& other):
    node_(other.node_)
  {}

  template <class T>
  LIter<T>& LIter<T>::operator=(const LIter& other)
  {
    if (this != &other)
    {
      node_ = other.node_;
    }
    return *this;
  }

  template <class T>
  LIter<T>::LIter(Node* node):
    node_(node)
  {}

  template <class T>
  T& LIter<T>::operator*() const
  {
    return node_->data_;
  }

  template <class T>
  T* LIter<T>::operator->() const
  {
    return &node_->data_;
  }

  template <class T>
  LIter<T>& LIter<T>::operator++()
  {
    if (node_ != nullptr)
    {
      node_ = node_->next_;
    }
    return *this;
  }

  template <class T>
  LIter<T> LIter<T>::operator++(int)
  {
    LIter old(*this);
    ++(*this);
    return old;
  }

  template <class T>
  LIter<T>& LIter<T>::operator--()
  {
    if (node_ != nullptr)
    {
      node_ = node_->prev_;
    }
    return *this;
  }

  template <class T>
  LIter<T> LIter<T>::operator--(int)
  {
    LIter old(*this);
    --(*this);
    return old;
  }

  template <class T>
  bool LIter<T>::operator==(const LIter& other) const
  {
    return node_ == other.node_;
  }

  template <class T>
  bool LIter<T>::operator!=(const LIter& other) const
  {
    return !(*this == other);
  }

  template <class T>
  LCIter<T>::LCIter():
    node_(nullptr)
  {}

  template <class T>
  LCIter<T>::LCIter(const LCIter& other):
    node_(other.node_)
  {}

  template <class T>
  LCIter<T>& LCIter<T>::operator=(const LCIter& other)
  {
    if (this != &other)
    {
      node_ = other.node_;
    }
    return *this;
  }

  template <class T>
  LCIter<T>::LCIter(const Node* node):
    node_(node)
  {}

  template <class T>
  const T& LCIter<T>::operator*() const
  {
    return node_->data_;
  }

  template <class T>
  const T* LCIter<T>::operator->() const
  {
    return &node_->data_;
  }

  template <class T>
  LCIter<T>& LCIter<T>::operator++()
  {
    if (node_ != nullptr)
    {
      node_ = node_->next_;
    }
    return *this;
  }

  template <class T>
  LCIter<T> LCIter<T>::operator++(int)
  {
    LCIter old(*this);
    ++(*this);
    return old;
  }

  template <class T>
  LCIter<T>& LCIter<T>::operator--()
  {
    if (node_ != nullptr)
    {
      node_ = node_->prev_;
    }
    return *this;
  }

  template <class T>
  LCIter<T> LCIter<T>::operator--(int)
  {
    LCIter old(*this);
    --(*this);
    return old;
  }

  template <class T>
  bool LCIter<T>::operator==(const LCIter& other) const
  {
    return node_ == other.node_;
  }

  template <class T>
  bool LCIter<T>::operator!=(const LCIter& other) const
  {
    return !(*this == other);
  }
}

#endif

#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>

namespace borisov {

template <class T>
class List {
public:
  using iterator = LIter<T>;
  using const_iterator = LCIter<T>;

  List();
  List(const List& other);
  List(List&& other) noexcept;
  ~List();

  List& operator=(const List& other);
  List& operator=(List&& other) noexcept;

  void push_front(const T& value);
  void push_back(const T& value);
  void pop_front();
  void pop_back();

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

private:
  struct Node {
    T data;
    Node* prev;
    Node* next;

    Node(const T& val)
      : data(val)
      , prev(nullptr)
      , next(nullptr)
    {}
  };

  Node* head_;
  Node* tail_;
  std::size_t size_;

  void swap(List& other) noexcept;
};

template <class T>
class LIter {
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
  typedef typename List<T>::Node Node;
  Node* node_;

  explicit LIter(Node* node);
};

template <class T>
class LCIter {
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
LIter<T>::LIter()
  : node_(nullptr)
{}

template <class T>
LIter<T>::LIter(const LIter& other)
  : node_(other.node_)
{}

template <class T>
LIter<T>& LIter<T>::operator=(const LIter& other) {
  if (this != &other) {
    node_ = other.node_;
  }
  return *this;
}

template <class T>
LIter<T>::LIter(Node* node)
  : node_(node)
{}

template <class T>
T& LIter<T>::operator*() const {
  return node_->data;
}

template <class T>
T* LIter<T>::operator->() const {
  return &node_->data;
}

template <class T>
LIter<T>& LIter<T>::operator++() {
  if (node_ != nullptr) {
    node_ = node_->next;
  }
  return *this;
}

template <class T>
LIter<T> LIter<T>::operator++(int) {
  LIter old = *this;
  ++(*this);
  return old;
}

template <class T>
LIter<T>& LIter<T>::operator--() {
  if (node_ != nullptr) {
    node_ = node_->prev;
  }
  return *this;
}

template <class T>
LIter<T> LIter<T>::operator--(int) {
  LIter old = *this;
  --(*this);
  return old;
}

template <class T>
bool LIter<T>::operator==(const LIter& other) const {
  return node_ == other.node_;
}

template <class T>
bool LIter<T>::operator!=(const LIter& other) const {
  return !(*this == other);
}

template <class T>
LCIter<T>::LCIter()
  : node_(nullptr)
{}

template <class T>
LCIter<T>::LCIter(const LCIter& other)
  : node_(other.node_)
{}

template <class T>
LCIter<T>& LCIter<T>::operator=(const LCIter& other) {
  if (this != &other) {
    node_ = other.node_;
  }
  return *this;
}

template <class T>
LCIter<T>::LCIter(const Node* node)
  : node_(node)
{}

template <class T>
const T& LCIter<T>::operator*() const {
  return node_->data;
}

template <class T>
const T* LCIter<T>::operator->() const {
  return &node_->data;
}

template <class T>
LCIter<T>& LCIter<T>::operator++() {
  if (node_ != nullptr) {
    node_ = node_->next;
  }
  return *this;
}

template <class T>
LCIter<T> LCIter<T>::operator++(int) {
  LCIter old = *this;
  ++(*this);
  return old;
}

template <class T>
LCIter<T>& LCIter<T>::operator--() {
  if (node_ != nullptr) {
    node_ = node_->prev;
  }
  return *this;
}

template <class T>
LCIter<T> LCIter<T>::operator--(int) {
  LCIter old = *this;
  --(*this);
  return old;
}

template <class T>
bool LCIter<T>::operator==(const LCIter& other) const {
  return node_ == other.node_;
}

template <class T>
bool LCIter<T>::operator!=(const LCIter& other) const {
  return !(*this == other);
}

template <class T>
List<T>::List()
  : head_(nullptr)
  , tail_(nullptr)
  , size_(0)
{}

template <class T>
List<T>::List(const List& other)
  : head_(nullptr)
  , tail_(nullptr)
  , size_(0)
{
  for (const_iterator it = other.begin(); it != other.end(); ++it) {
    push_back(*it);
  }
}

template <class T>
List<T>::List(List&& other) noexcept
  : head_(other.head_)
  , tail_(other.tail_)
  , size_(other.size_)
{
  other.head_ = nullptr;
  other.tail_ = nullptr;
  other.size_ = 0;
}

template <class T>
bool List<T>::empty() const {
  return size_ == 0;
}

template <class T>
List<T>::~List() {
  clear();
}

template <class T>
void List<T>::clear() {
  while (!empty()) {
    pop_front();
  }
}

}

#endif
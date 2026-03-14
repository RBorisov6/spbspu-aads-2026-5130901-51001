#include <utility>

namespace borisov {

template <typename T>
template <typename... Args>
List<T>::Node::Node(Args&&... args)
  : data(std::forward<Args>(args)...)
  , prev(nullptr)
  , next(nullptr)
{
}

template <typename T>
void List<T>::create_sentinel_nodes()
{
  m_head = new Node();
  m_tail = new Node();
  m_head->next = m_tail;
  m_tail->prev = m_head;
}

template <typename T>
List<T>::List()
  : m_head(nullptr)
  , m_tail(nullptr)
  , m_size(0)
{
  create_sentinel_nodes();
}

template <typename T>
List<T>::~List()
{
  Node* current = m_head;
  while (current) {
    Node* next = current->next;
    delete current;
    current = next;
  }
}

}
#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>

namespace borisov {

template <class T>
class List {
private:
	struct Node {
		T data;
		Node* prev;
		Node* next;

		template <class... Args>
		explicit Node(Args&&... args);
	};

	Node* m_head;
	Node* m_tail;
	std::size_t m_size;

	void create_sentinel_nodes();

public:
	List();
	~List();

	List(const List&) = delete;
	List(List&&) = delete;
	List& operator=(const List&) = delete;
  List& operator=(List&&) = delete;
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

}

#include "list.cpp"

#endif
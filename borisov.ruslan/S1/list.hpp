#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>

namespace borisov {

template <typename T>
class List {
private:
	struct Node {
		T data;
		Node* prev;
		Node* next;

		template <typename... Args>
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

}

#include "list.cpp"

#endif
#include <boost/test/unit_test.hpp>
#include "../common/list.hpp"
#include "stack.hpp"
#include "queue.hpp"

namespace
{
  struct Point
  {
    int x_;
    int y_;

    Point(int x, int y):
      x_(x),
      y_(y)
    {}
  };

  struct Tracker
  {
    int copies_;
    int moves_;

    Tracker():
      copies_(0),
      moves_(0)
    {}

    Tracker(const Tracker& other):
      copies_(other.copies_ + 1),
      moves_(other.moves_)
    {}

    Tracker(Tracker&& other):
      copies_(other.copies_),
      moves_(other.moves_ + 1)
    {}

    Tracker& operator=(const Tracker&) = default;
    Tracker& operator=(Tracker&&) = default;
  };

  struct MoveOnly
  {
    int val_;

    explicit MoveOnly(int v):
      val_(v)
    {}

    MoveOnly(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly& operator=(MoveOnly&&) = default;
  };
}

BOOST_AUTO_TEST_CASE(list_emplace_back_multi_arg)
{
  borisov::List< Point > list;
  list.emplaceBack(1, 2);
  BOOST_CHECK_EQUAL(list.back().x_, 1);
  BOOST_CHECK_EQUAL(list.back().y_, 2);
  BOOST_CHECK_EQUAL(list.size(), 1u);
}

BOOST_AUTO_TEST_CASE(list_emplace_front_multi_arg)
{
  borisov::List< Point > list;
  list.emplaceBack(3, 4);
  list.emplaceFront(1, 2);
  BOOST_CHECK_EQUAL(list.front().x_, 1);
  BOOST_CHECK_EQUAL(list.front().y_, 2);
  BOOST_CHECK_EQUAL(list.back().x_, 3);
  BOOST_CHECK_EQUAL(list.back().y_, 4);
  BOOST_CHECK_EQUAL(list.size(), 2u);
}

BOOST_AUTO_TEST_CASE(list_emplace_at_middle)
{
  borisov::List< Point > list;
  list.emplaceBack(1, 1);
  list.emplaceBack(3, 3);
  auto it = list.begin();
  ++it;
  list.emplace(it, 2, 2);
  BOOST_CHECK_EQUAL(list.size(), 3u);
  auto cur = list.begin();
  BOOST_CHECK_EQUAL(cur->x_, 1);
  ++cur;
  BOOST_CHECK_EQUAL(cur->x_, 2);
  ++cur;
  BOOST_CHECK_EQUAL(cur->x_, 3);
}

BOOST_AUTO_TEST_CASE(list_emplace_back_rvalue_moves_not_copies)
{
  borisov::List< Tracker > list;
  list.emplaceBack(Tracker{});
  BOOST_CHECK_EQUAL(list.back().moves_, 1);
  BOOST_CHECK_EQUAL(list.back().copies_, 0);
}

BOOST_AUTO_TEST_CASE(list_emplace_back_lvalue_copies)
{
  borisov::List< Tracker > list;
  const Tracker t;
  list.emplaceBack(t);
  BOOST_CHECK_EQUAL(list.back().copies_, 1);
  BOOST_CHECK_EQUAL(list.back().moves_, 0);
}

BOOST_AUTO_TEST_CASE(list_emplace_front_rvalue_moves_not_copies)
{
  borisov::List< Tracker > list;
  list.emplaceFront(Tracker{});
  BOOST_CHECK_EQUAL(list.front().moves_, 1);
  BOOST_CHECK_EQUAL(list.front().copies_, 0);
}

BOOST_AUTO_TEST_CASE(list_emplace_back_move_only_type)
{
  borisov::List< MoveOnly > list;
  list.emplaceBack(42);
  BOOST_CHECK_EQUAL(list.back().val_, 42);
}

BOOST_AUTO_TEST_CASE(list_emplace_front_move_only_type)
{
  borisov::List< MoveOnly > list;
  list.emplaceFront(7);
  BOOST_CHECK_EQUAL(list.front().val_, 7);
}

BOOST_AUTO_TEST_CASE(stack_emplace_multi_arg)
{
  borisov::Stack< Point > s;
  s.emplace(10, 20);
  BOOST_CHECK_EQUAL(s.top().x_, 10);
  BOOST_CHECK_EQUAL(s.top().y_, 20);
  BOOST_CHECK_EQUAL(s.size(), 1u);
}

BOOST_AUTO_TEST_CASE(stack_emplace_rvalue_moves_not_copies)
{
  borisov::Stack< Tracker > s;
  s.emplace(Tracker{});
  BOOST_CHECK_EQUAL(s.top().moves_, 1);
  BOOST_CHECK_EQUAL(s.top().copies_, 0);
}

BOOST_AUTO_TEST_CASE(stack_emplace_move_only_type)
{
  borisov::Stack< MoveOnly > s;
  s.emplace(7);
  BOOST_CHECK_EQUAL(s.top().val_, 7);
}

BOOST_AUTO_TEST_CASE(stack_emplace_lifo_order)
{
  borisov::Stack< Point > s;
  s.emplace(1, 1);
  s.emplace(2, 2);
  BOOST_CHECK_EQUAL(s.top().x_, 2);
  s.pop();
  BOOST_CHECK_EQUAL(s.top().x_, 1);
}

BOOST_AUTO_TEST_CASE(queue_emplace_multi_arg)
{
  borisov::Queue< Point > q;
  q.emplace(5, 6);
  BOOST_CHECK_EQUAL(q.back().x_, 5);
  BOOST_CHECK_EQUAL(q.back().y_, 6);
  BOOST_CHECK_EQUAL(q.size(), 1u);
}

BOOST_AUTO_TEST_CASE(queue_emplace_rvalue_moves_not_copies)
{
  borisov::Queue< Tracker > q;
  q.emplace(Tracker{});
  BOOST_CHECK_EQUAL(q.back().moves_, 1);
  BOOST_CHECK_EQUAL(q.back().copies_, 0);
}

BOOST_AUTO_TEST_CASE(queue_emplace_move_only_type)
{
  borisov::Queue< MoveOnly > q;
  q.emplace(99);
  BOOST_CHECK_EQUAL(q.front().val_, 99);
}

BOOST_AUTO_TEST_CASE(queue_emplace_fifo_order)
{
  borisov::Queue< Point > q;
  q.emplace(1, 1);
  q.emplace(2, 2);
  BOOST_CHECK_EQUAL(q.front().x_, 1);
  q.pop();
  BOOST_CHECK_EQUAL(q.front().x_, 2);
}

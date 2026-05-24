#include <boost/test/unit_test.hpp>
#include "queue.hpp"

BOOST_AUTO_TEST_CASE(queue_push_and_front)
{
  borisov::Queue<int> q;
  q.push(10);
  BOOST_CHECK_EQUAL(q.front(), 10);
  q.push(20);
  BOOST_CHECK_EQUAL(q.front(), 10);  // front remains first added
}

BOOST_AUTO_TEST_CASE(queue_pop_fifo_order)
{
  borisov::Queue<int> q;
  q.push(1);
  q.push(2);
  q.pop();
  BOOST_CHECK_EQUAL(q.front(), 2);
  q.pop();
  BOOST_CHECK(q.empty());
}

BOOST_AUTO_TEST_CASE(queue_back_reflects_last)
{
  borisov::Queue<int> q;
  q.push(100);
  q.push(200);
  BOOST_CHECK_EQUAL(q.back(), 200);
  q.pop();
  BOOST_CHECK_EQUAL(q.back(), 200); // back unchanged after pop front
}

BOOST_AUTO_TEST_CASE(queue_empty_initially)
{
  borisov::Queue<int> q;
  BOOST_CHECK(q.empty());
}

BOOST_AUTO_TEST_CASE(queue_pop_on_empty_throws)
{
  borisov::Queue<int> q;
  BOOST_CHECK_THROW(q.pop(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(queue_front_on_empty_throws)
{
  borisov::Queue<int> q;
  BOOST_CHECK_THROW(q.front(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(queue_back_on_empty_throws)
{
  borisov::Queue<int> q;
  BOOST_CHECK_THROW(q.back(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(queue_size_reflects_elements)
{
  borisov::Queue<int> q;
  BOOST_CHECK_EQUAL(q.size(), 0u);
  q.push(1);
  BOOST_CHECK_EQUAL(q.size(), 1u);
  q.push(2);
  BOOST_CHECK_EQUAL(q.size(), 2u);
  q.pop();
  BOOST_CHECK_EQUAL(q.size(), 1u);
}

BOOST_AUTO_TEST_CASE(queue_clear_empties)
{
  borisov::Queue<int> q;
  q.push(3);
  q.push(4);
  q.clear();
  BOOST_CHECK(q.empty());
  BOOST_CHECK_EQUAL(q.size(), 0u);
}

BOOST_AUTO_TEST_CASE(queue_copy_constructor_independent)
{
  borisov::Queue<int> original;
  original.push(10);
  original.push(20);
  borisov::Queue<int> copy(original);
  BOOST_CHECK_EQUAL(copy.size(), original.size());
  BOOST_CHECK_EQUAL(copy.front(), original.front());
  copy.pop();
  BOOST_CHECK_EQUAL(copy.size(), original.size() - 1);
  BOOST_CHECK_EQUAL(original.front(), 10); // original unchanged
}

BOOST_AUTO_TEST_CASE(queue_copy_assignment_replaces)
{
  borisov::Queue<int> q1;
  q1.push(1);
  q1.push(2);

  borisov::Queue<int> q2;
  q2.push(100);
  q2 = q1;
  BOOST_CHECK_EQUAL(q2.size(), 2);
  BOOST_CHECK_EQUAL(q2.back(), 2);
  q1.pop();
  BOOST_CHECK_EQUAL(q2.size(), 2); // q2 independent
}

BOOST_AUTO_TEST_CASE(queue_move_constructor_transfers)
{
  borisov::Queue<int> source;
  source.push(5);
  source.push(6);
  borisov::Queue<int> target(std::move(source));
  BOOST_CHECK(source.empty());
  BOOST_CHECK_EQUAL(target.size(), 2);
  BOOST_CHECK_EQUAL(target.front(), 5);
  BOOST_CHECK_EQUAL(target.back(), 6);
}

BOOST_AUTO_TEST_CASE(queue_move_assignment_transfers)
{
  borisov::Queue<int> source;
  source.push(99);

  borisov::Queue<int> target;
  target.push(200);
  target = std::move(source);
  BOOST_CHECK(source.empty());
  BOOST_CHECK_EQUAL(target.size(), 1);
  BOOST_CHECK_EQUAL(target.front(), 99);
}

BOOST_AUTO_TEST_CASE(queue_self_assignment_ok)
{
  borisov::Queue<int> q;
  q.push(45);
  q = q;
  BOOST_CHECK_EQUAL(q.size(), 1);
  BOOST_CHECK_EQUAL(q.front(), 45);
  q = std::move(q);
  BOOST_CHECK_EQUAL(q.size(), 1);
  BOOST_CHECK_EQUAL(q.front(), 45);
}

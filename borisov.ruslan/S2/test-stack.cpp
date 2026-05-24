#include <boost/test/unit_test.hpp>
#include "stack.hpp"

BOOST_AUTO_TEST_CASE(stack_push_and_top)
{
  borisov::Stack<int> s;
  s.push(10);
  BOOST_CHECK_EQUAL(s.top(), 10);
  s.push(20);
  BOOST_CHECK_EQUAL(s.top(), 20);
}

BOOST_AUTO_TEST_CASE(stack_pop_removes_top)
{
  borisov::Stack<int> s;
  s.push(1);
  s.push(2);
  s.pop();
  BOOST_CHECK_EQUAL(s.top(), 1);
  s.pop();
  BOOST_CHECK(s.empty());
}

BOOST_AUTO_TEST_CASE(stack_empty_initially)
{
  borisov::Stack<int> s;
  BOOST_CHECK(s.empty());
}

BOOST_AUTO_TEST_CASE(stack_pop_on_empty_throws)
{
  borisov::Stack<int> s;
  BOOST_CHECK_THROW(s.pop(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(stack_top_on_empty_throws)
{
  borisov::Stack<int> s;
  BOOST_CHECK_THROW(s.top(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(stack_size_after_push)
{
  borisov::Stack<int> s;
  BOOST_CHECK_EQUAL(s.size(), 0u);
  s.push(100);
  BOOST_CHECK_EQUAL(s.size(), 1u);
  s.push(200);
  BOOST_CHECK_EQUAL(s.size(), 2u);
}

BOOST_AUTO_TEST_CASE(stack_size_after_pop)
{
  borisov::Stack<int> s;
  s.push(1);
  s.push(2);
  s.pop();
  BOOST_CHECK_EQUAL(s.size(), 1u);
  s.pop();
  BOOST_CHECK_EQUAL(s.size(), 0u);
}

BOOST_AUTO_TEST_CASE(stack_clear_makes_empty)
{
  borisov::Stack<int> s;
  s.push(5);
  s.push(10);
  s.clear();
  BOOST_CHECK(s.empty());
  BOOST_CHECK_EQUAL(s.size(), 0u);
}

BOOST_AUTO_TEST_CASE(stack_copy_constructor_creates_independent_copy)
{
  borisov::Stack<int> original;
  original.push(10);
  original.push(20);
  borisov::Stack<int> copy(original);
  BOOST_CHECK_EQUAL(copy.size(), original.size());
  BOOST_CHECK_EQUAL(copy.top(), original.top());
  copy.pop();
  BOOST_CHECK_EQUAL(copy.size(), original.size() - 1);
  BOOST_CHECK_EQUAL(original.top(), 20); // original unchanged
}

BOOST_AUTO_TEST_CASE(stack_copy_assignment_replaces_content)
{
  borisov::Stack<int> s1;
  s1.push(1);
  s1.push(2);

  borisov::Stack<int> s2;
  s2.push(100);

  s2 = s1;
  BOOST_CHECK_EQUAL(s2.size(), 2);
  BOOST_CHECK_EQUAL(s2.top(), 2);
  s1.pop();
  BOOST_CHECK_EQUAL(s2.size(), 2); // s2 independent
}

BOOST_AUTO_TEST_CASE(stack_move_constructor_transfers_ownership)
{
  borisov::Stack<int> source;
  source.push(42);
  source.push(99);
  borisov::Stack<int> target(std::move(source));
  BOOST_CHECK(source.empty());
  BOOST_CHECK_EQUAL(target.size(), 2);
  BOOST_CHECK_EQUAL(target.top(), 99);
}

BOOST_AUTO_TEST_CASE(stack_move_assignment_transfers_ownership)
{
  borisov::Stack<int> source;
  source.push(7);

  borisov::Stack<int> target;
  target.push(1000);

  target = std::move(source);
  BOOST_CHECK(source.empty());
  BOOST_CHECK_EQUAL(target.size(), 1);
  BOOST_CHECK_EQUAL(target.top(), 7);
}

BOOST_AUTO_TEST_CASE(stack_self_assignment_does_nothing)
{
  borisov::Stack<int> s;
  s.push(5);
  s = s;
  BOOST_CHECK_EQUAL(s.size(), 1);
  BOOST_CHECK_EQUAL(s.top(), 5);
  s = std::move(s);
  BOOST_CHECK_EQUAL(s.size(), 1);
  BOOST_CHECK_EQUAL(s.top(), 5);
}

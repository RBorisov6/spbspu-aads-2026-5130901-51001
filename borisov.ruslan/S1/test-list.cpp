#define BOOST_TEST_MODULE S1_Tests
#include <boost/test/included/unit_test.hpp>
#include "list.hpp"

BOOST_AUTO_TEST_CASE(test_default_constructor)
{
  borisov::List<int> lst;
  BOOST_CHECK(lst.empty());
  BOOST_CHECK_EQUAL(lst.size(), 0u);
  BOOST_CHECK(lst.begin() == lst.end());
}

BOOST_AUTO_TEST_CASE(test_push_front_and_back)
{
  borisov::List<int> lst;
  lst.pushFront(2);
  lst.pushBack(3);
  lst.pushFront(1);

  auto it = lst.begin();
  BOOST_CHECK_EQUAL(*it++, 1);
  BOOST_CHECK_EQUAL(*it++, 2);
  BOOST_CHECK_EQUAL(*it, 3);
  BOOST_CHECK_EQUAL(lst.size(), 3u);
}

BOOST_AUTO_TEST_CASE(test_pop_front_and_back)
{
  borisov::List<int> lst;
  lst.pushBack(10);
  lst.pushBack(20);
  lst.pushBack(30);

  lst.popFront();
  BOOST_CHECK_EQUAL(lst.front(), 20);
  BOOST_CHECK_EQUAL(lst.size(), 2u);

  lst.popBack();
  BOOST_CHECK_EQUAL(lst.back(), 20);
  BOOST_CHECK_EQUAL(lst.size(), 1u);

  lst.popFront();
  BOOST_CHECK(lst.empty());
}

BOOST_AUTO_TEST_CASE(test_clear)
{
  borisov::List<int> lst;
  lst.pushBack(1);
  lst.pushBack(2);
  lst.clear();
  BOOST_CHECK(lst.empty());
  BOOST_CHECK_EQUAL(lst.size(), 0u);
}

BOOST_AUTO_TEST_CASE(test_copy_constructor)
{
  borisov::List<int> original;
  original.pushBack(5);
  original.pushBack(6);
  borisov::List<int> copy(original);
  copy.popFront();
  BOOST_CHECK_EQUAL(original.size(), 2u);
  BOOST_CHECK_EQUAL(copy.size(), 1u);
  BOOST_CHECK_EQUAL(copy.front(), 6);
}

BOOST_AUTO_TEST_CASE(test_copy_assignment)
{
  borisov::List<int> a;
  a.pushBack(100);
  borisov::List<int> b;
  b = a;
  b.pushFront(200);
  BOOST_CHECK_EQUAL(a.size(), 1u);
  BOOST_CHECK_EQUAL(b.size(), 2u);
}

BOOST_AUTO_TEST_CASE(test_move_constructor)
{
  borisov::List<int> a;
  a.pushBack(7);
  a.pushBack(8);
  borisov::List<int> b(std::move(a));
  BOOST_CHECK(a.empty());
  BOOST_CHECK_EQUAL(b.size(), 2u);
  BOOST_CHECK_EQUAL(b.front(), 7);
  BOOST_CHECK_EQUAL(b.back(), 8);
}

BOOST_AUTO_TEST_CASE(test_move_assignment)
{
  borisov::List<int> a;
  a.pushBack(42);
  borisov::List<int> b;
  b = std::move(a);
  BOOST_CHECK(a.empty());
  BOOST_CHECK_EQUAL(b.size(), 1u);
  BOOST_CHECK_EQUAL(b.front(), 42);
}

BOOST_AUTO_TEST_CASE(test_swap)
{
  borisov::List<int> lst1;
  lst1.pushBack(1);
  lst1.pushBack(2);

  borisov::List<int> lst2;
  lst2.pushBack(10);
  lst2.pushBack(20);
  lst2.pushBack(30);

  lst1.swap(lst2);

  BOOST_CHECK_EQUAL(lst1.size(), 3u);
  BOOST_CHECK_EQUAL(lst1.front(), 10);
  BOOST_CHECK_EQUAL(lst2.size(), 2u);
  BOOST_CHECK_EQUAL(lst2.front(), 1);
}

BOOST_AUTO_TEST_CASE(test_iterator_postfix_increment)
{
  borisov::List<int> lst;
  lst.pushBack(4);
  lst.pushBack(5);
  lst.pushBack(6);

  auto it = lst.cbegin();
  BOOST_CHECK_EQUAL(*it++, 4);
  BOOST_CHECK_EQUAL(*it++, 5);
  BOOST_CHECK_EQUAL(*it, 6);
  ++it;
  BOOST_CHECK(it == lst.cend());
}

BOOST_AUTO_TEST_CASE(test_insert_and_erase)
{
  borisov::List<int> lst;
  lst.pushBack(10);
  lst.pushBack(30);

  auto it = lst.begin();
  ++it;
  lst.insert(it, 20);  // {10, 20, 30}

  BOOST_CHECK_EQUAL(lst.size(), 3u);
  it = lst.begin();
  BOOST_CHECK_EQUAL(*it++, 10);
  BOOST_CHECK_EQUAL(*it++, 20);
  BOOST_CHECK_EQUAL(*it, 30);

  it = lst.begin();
  ++it;
  lst.erase(it);       // удаляем 20 -> {10, 30}
  BOOST_CHECK_EQUAL(lst.size(), 2u);
  it = lst.begin();
  BOOST_CHECK_EQUAL(*it++, 10);
  BOOST_CHECK_EQUAL(*it, 30);
}

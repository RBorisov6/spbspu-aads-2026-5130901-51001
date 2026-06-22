#include <boost/test/unit_test.hpp>
#include "bstree.hpp"
#include <stdexcept>

BOOST_AUTO_TEST_CASE(bst_push_and_get)
{
  borisov::BSTree< int, std::string > t;
  t.push(1, "one");
  BOOST_CHECK_EQUAL(t.get(1), "one");
}

BOOST_AUTO_TEST_CASE(bst_get_missing_throws)
{
  borisov::BSTree< int, std::string > t;
  BOOST_CHECK_THROW(t.get(42), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(bst_has)
{
  borisov::BSTree< int, std::string > t;
  t.push(5, "five");
  BOOST_CHECK(t.has(5));
  BOOST_CHECK(!t.has(3));
}

BOOST_AUTO_TEST_CASE(bst_drop_leaf)
{
  borisov::BSTree< int, std::string > t;
  t.push(1, "one");
  BOOST_CHECK_EQUAL(t.drop(1), "one");
  BOOST_CHECK(!t.has(1));
  BOOST_CHECK(t.empty());
}

BOOST_AUTO_TEST_CASE(bst_drop_two_children)
{
  borisov::BSTree< int, std::string > t;
  t.push(2, "two");
  t.push(1, "one");
  t.push(3, "three");
  t.drop(2);
  BOOST_CHECK(t.has(1));
  BOOST_CHECK(t.has(3));
  BOOST_CHECK(!t.has(2));
}

BOOST_AUTO_TEST_CASE(bst_drop_missing_throws)
{
  borisov::BSTree< int, std::string > t;
  BOOST_CHECK_THROW(t.drop(7), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(bst_size)
{
  borisov::BSTree< int, std::string > t;
  BOOST_CHECK_EQUAL(t.size(), 0u);
  t.push(1, "a");
  BOOST_CHECK_EQUAL(t.size(), 1u);
  t.push(2, "b");
  BOOST_CHECK_EQUAL(t.size(), 2u);
}

BOOST_AUTO_TEST_CASE(bst_empty)
{
  borisov::BSTree< int, std::string > t;
  BOOST_CHECK(t.empty());
  t.push(1, "a");
  BOOST_CHECK(!t.empty());
}

BOOST_AUTO_TEST_CASE(bst_push_duplicate_updates_value)
{
  borisov::BSTree< int, std::string > t;
  t.push(1, "one");
  t.push(1, "ONE");
  BOOST_CHECK_EQUAL(t.get(1), "ONE");
  BOOST_CHECK_EQUAL(t.size(), 1u);
}

BOOST_AUTO_TEST_CASE(bst_clear)
{
  borisov::BSTree< int, std::string > t;
  t.push(1, "a");
  t.push(2, "b");
  t.clear();
  BOOST_CHECK(t.empty());
}

BOOST_AUTO_TEST_CASE(bst_copy)
{
  borisov::BSTree< int, std::string > t;
  t.push(3, "three");
  t.push(1, "one");
  t.push(5, "five");
  borisov::BSTree< int, std::string > t2(t);
  BOOST_CHECK_EQUAL(t2.get(1), "one");
  BOOST_CHECK_EQUAL(t2.get(3), "three");
  BOOST_CHECK_EQUAL(t2.get(5), "five");
}

BOOST_AUTO_TEST_CASE(bst_assign_copy)
{
  borisov::BSTree< int, std::string > t;
  t.push(10, "ten");
  borisov::BSTree< int, std::string > t2;
  t2 = t;
  BOOST_CHECK_EQUAL(t2.get(10), "ten");
}

BOOST_AUTO_TEST_CASE(bst_inorder_traversal)
{
  borisov::BSTree< int, std::string > t;
  t.push(3, "c");
  t.push(1, "a");
  t.push(5, "e");
  t.push(2, "b");
  t.push(4, "d");
  int prev = -1;
  for (auto it = t.begin(); it != t.end(); ++it)
  {
    BOOST_CHECK(it->first > prev);
    prev = it->first;
  }
  BOOST_CHECK_EQUAL(prev, 5);
}

BOOST_AUTO_TEST_CASE(bst_empty_begin_equals_end)
{
  borisov::BSTree< int, std::string > t;
  BOOST_CHECK(t.begin() == t.end());
}

BOOST_AUTO_TEST_CASE(bst_const_iterator)
{
  borisov::BSTree< int, std::string > t;
  t.push(1, "a");
  t.push(2, "b");
  const borisov::BSTree< int, std::string >& ct = t;
  int count = 0;
  for (auto it = ct.cbegin(); it != ct.cend(); ++it)
  {
    ++count;
  }
  BOOST_CHECK_EQUAL(count, 2);
}

BOOST_AUTO_TEST_CASE(bst_find)
{
  borisov::BSTree< int, std::string > t;
  t.push(7, "seven");
  auto it = t.find(7);
  BOOST_CHECK(it != t.end());
  BOOST_CHECK_EQUAL(it->first, 7);
}

BOOST_AUTO_TEST_CASE(bst_find_missing_returns_end)
{
  borisov::BSTree< int, std::string > t;
  BOOST_CHECK(t.find(99) == t.end());
}

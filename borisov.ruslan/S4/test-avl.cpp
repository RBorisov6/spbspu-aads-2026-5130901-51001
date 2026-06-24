#include <boost/test/unit_test.hpp>
#include "avltree.hpp"
#include "treemap.hpp"
#include "treeset.hpp"
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE(avl_push_and_has)
{
  borisov::AVLTree< int, std::string > t;
  t.push(3, "three");
  t.push(1, "one");
  t.push(5, "five");
  BOOST_CHECK(t.has(1));
  BOOST_CHECK(t.has(3));
  BOOST_CHECK(t.has(5));
  BOOST_CHECK(!t.has(0));
}

BOOST_AUTO_TEST_CASE(avl_push_replaces_value)
{
  borisov::AVLTree< int, std::string > t;
  t.push(1, "one");
  t.push(1, "ONE");
  BOOST_CHECK_EQUAL(t.get(1), "ONE");
  BOOST_CHECK_EQUAL(t.size(), 1u);
}

BOOST_AUTO_TEST_CASE(avl_get_missing_throws)
{
  borisov::AVLTree< int, std::string > t;
  BOOST_CHECK_THROW(t.get(42), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(avl_drop)
{
  borisov::AVLTree< int, std::string > t;
  t.push(2, "two");
  t.push(1, "one");
  t.push(3, "three");
  t.drop(2);
  BOOST_CHECK(!t.has(2));
  BOOST_CHECK(t.has(1));
  BOOST_CHECK(t.has(3));
  BOOST_CHECK_EQUAL(t.size(), 2u);
}

BOOST_AUTO_TEST_CASE(avl_drop_missing_throws)
{
  borisov::AVLTree< int, std::string > t;
  BOOST_CHECK_THROW(t.drop(99), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(avl_size_empty)
{
  borisov::AVLTree< int, int > t;
  BOOST_CHECK(t.empty());
  BOOST_CHECK_EQUAL(t.size(), 0u);
  t.push(1, 1);
  BOOST_CHECK(!t.empty());
  BOOST_CHECK_EQUAL(t.size(), 1u);
}

BOOST_AUTO_TEST_CASE(avl_inorder_iteration)
{
  borisov::AVLTree< int, int > t;
  t.push(5, 0);
  t.push(2, 0);
  t.push(8, 0);
  t.push(1, 0);
  t.push(4, 0);
  t.push(7, 0);
  t.push(9, 0);
  std::vector< int > keys;
  for (auto it = t.cbegin(); it != t.cend(); ++it)
  {
    keys.push_back(it->first);
  }
  for (std::size_t i = 1; i < keys.size(); ++i)
  {
    BOOST_CHECK_LT(keys[i - 1], keys[i]);
  }
}

BOOST_AUTO_TEST_CASE(avl_balanced_height)
{
  borisov::AVLTree< int, int > t;
  for (int i = 1; i <= 64; ++i)
  {
    t.push(i, i);
  }
  BOOST_CHECK_EQUAL(t.size(), 64u);
  // AVL guarantee: height <= 1.44 * log2(N+2); for N=64 that is < 10
  BOOST_CHECK_LE(t.height(), 10u);
}

BOOST_AUTO_TEST_CASE(avl_copy)
{
  borisov::AVLTree< int, std::string > t;
  t.push(1, "a");
  t.push(2, "b");
  borisov::AVLTree< int, std::string > t2(t);
  BOOST_CHECK_EQUAL(t2.get(1), "a");
  BOOST_CHECK_EQUAL(t2.get(2), "b");
  t2.push(1, "X");
  BOOST_CHECK_EQUAL(t.get(1), "a");
}

BOOST_AUTO_TEST_CASE(treemap_basic)
{
  borisov::TreeMap< std::string, int > m;
  m.insert("apple", 3);
  m.insert("banana", 1);
  m.insert("cherry", 2);
  BOOST_CHECK_EQUAL(m.at("apple"), 3);
  BOOST_CHECK_EQUAL(m.at("banana"), 1);
  BOOST_CHECK_EQUAL(m.size(), 3u);
}

BOOST_AUTO_TEST_CASE(treemap_operator_bracket)
{
  borisov::TreeMap< std::string, int > m;
  m["hello"] = 42;
  BOOST_CHECK_EQUAL(m["hello"], 42);
  BOOST_CHECK_EQUAL(m.size(), 1u);
  m["world"];
  BOOST_CHECK_EQUAL(m["world"], 0);
  BOOST_CHECK_EQUAL(m.size(), 2u);
}

BOOST_AUTO_TEST_CASE(treemap_erase)
{
  borisov::TreeMap< int, int > m;
  m.insert(1, 10);
  m.insert(2, 20);
  m.erase(1);
  BOOST_CHECK(!m.count(1));
  BOOST_CHECK(m.count(2));
}

BOOST_AUTO_TEST_CASE(treemap_iteration_sorted)
{
  borisov::TreeMap< int, int > m;
  m.insert(3, 0);
  m.insert(1, 0);
  m.insert(2, 0);
  int prev = -1;
  for (auto it = m.begin(); it != m.end(); ++it)
  {
    BOOST_CHECK_LT(prev, it->first);
    prev = it->first;
  }
}

BOOST_AUTO_TEST_CASE(treeset_basic)
{
  borisov::TreeSet< int > s;
  s.insert(3);
  s.insert(1);
  s.insert(2);
  BOOST_CHECK(s.count(1));
  BOOST_CHECK(s.count(2));
  BOOST_CHECK(s.count(3));
  BOOST_CHECK(!s.count(4));
  BOOST_CHECK_EQUAL(s.size(), 3u);
}

BOOST_AUTO_TEST_CASE(treeset_erase)
{
  borisov::TreeSet< std::string > s;
  s.insert("a");
  s.insert("b");
  s.erase("a");
  BOOST_CHECK(!s.count("a"));
  BOOST_CHECK(s.count("b"));
}

BOOST_AUTO_TEST_CASE(treeset_iteration_sorted)
{
  borisov::TreeSet< int > s;
  s.insert(5);
  s.insert(2);
  s.insert(8);
  s.insert(1);
  int prev = -1;
  for (auto it = s.begin(); it != s.end(); ++it)
  {
    BOOST_CHECK_LT(prev, *it);
    prev = *it;
  }
}

BOOST_AUTO_TEST_CASE(treeset_no_duplicates)
{
  borisov::TreeSet< int > s;
  s.insert(1);
  s.insert(1);
  s.insert(1);
  BOOST_CHECK_EQUAL(s.size(), 1u);
}

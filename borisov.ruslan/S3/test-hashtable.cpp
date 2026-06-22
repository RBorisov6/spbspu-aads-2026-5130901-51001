#include <boost/test/unit_test.hpp>
#include "hashtable.hpp"
#include "xxhash.hpp"
#include <string>
#include <stdexcept>

using HT = borisov::HashTable< std::string, int, borisov::XxHash32 >;

BOOST_AUTO_TEST_CASE(hashtable_empty_on_construction)
{
  HT t(8);
  BOOST_CHECK(t.empty());
  BOOST_CHECK_EQUAL(t.size(), 0u);
  BOOST_CHECK_EQUAL(t.slots(), 8u);
}

BOOST_AUTO_TEST_CASE(hashtable_add_and_has)
{
  HT t(8);
  t.add("key1", 42);
  BOOST_CHECK(t.has("key1"));
  BOOST_CHECK(!t.has("key2"));
}

BOOST_AUTO_TEST_CASE(hashtable_add_increments_size)
{
  HT t(8);
  t.add("a", 1);
  t.add("b", 2);
  BOOST_CHECK_EQUAL(t.size(), 2u);
}

BOOST_AUTO_TEST_CASE(hashtable_at_returns_value)
{
  HT t(8);
  t.add("hello", 99);
  BOOST_CHECK_EQUAL(t.at("hello"), 99);
}

BOOST_AUTO_TEST_CASE(hashtable_at_throws_for_missing_key)
{
  HT t(8);
  BOOST_CHECK_THROW(t.at("missing"), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(hashtable_drop_returns_value_and_removes)
{
  HT t(8);
  t.add("x", 7);
  int v = t.drop("x");
  BOOST_CHECK_EQUAL(v, 7);
  BOOST_CHECK(!t.has("x"));
  BOOST_CHECK_EQUAL(t.size(), 0u);
}

BOOST_AUTO_TEST_CASE(hashtable_drop_throws_for_missing_key)
{
  HT t(8);
  BOOST_CHECK_THROW(t.drop("ghost"), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(hashtable_overflow_throws_when_full)
{
  HT t(2);
  t.add("a", 1);
  t.add("b", 2);
  BOOST_CHECK_THROW(t.add("c", 3), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(hashtable_rehash_allows_more_inserts)
{
  HT t(2);
  t.add("a", 1);
  t.add("b", 2);
  t.rehash(10);
  BOOST_CHECK_EQUAL(t.slots(), 10u);
  BOOST_CHECK_EQUAL(t.size(), 2u);
  t.add("c", 3);
  BOOST_CHECK(t.has("c"));
}

BOOST_AUTO_TEST_CASE(hashtable_clear_empties_table)
{
  HT t(8);
  t.add("a", 1);
  t.add("b", 2);
  t.clear();
  BOOST_CHECK(t.empty());
  BOOST_CHECK(!t.has("a"));
}

BOOST_AUTO_TEST_CASE(hashtable_copy_constructor_independent)
{
  HT t1(8);
  t1.add("x", 10);
  HT t2(t1);
  t2.add("y", 20);
  BOOST_CHECK(!t1.has("y"));
  BOOST_CHECK(t2.has("x"));
}

BOOST_AUTO_TEST_CASE(hashtable_move_constructor)
{
  HT t1(8);
  t1.add("z", 5);
  HT t2(std::move(t1));
  BOOST_CHECK(t2.has("z"));
  BOOST_CHECK(t1.empty());
}

BOOST_AUTO_TEST_CASE(hashtable_multiple_in_same_bucket)
{
  HT t(1);
  t.add("aaa", 1);
  BOOST_CHECK_THROW(t.add("bbb", 2), std::overflow_error);
  t.rehash(4);
  t.add("bbb", 2);
  BOOST_CHECK(t.has("aaa"));
  BOOST_CHECK(t.has("bbb"));
}

BOOST_AUTO_TEST_CASE(hashtable_iterator_visits_all_elements)
{
  HT t(8);
  t.add("one", 1);
  t.add("two", 2);
  t.add("three", 3);
  int sum = 0;
  for (auto it = t.begin(); it != t.end(); ++it)
  {
    sum += it->second;
  }
  BOOST_CHECK_EQUAL(sum, 6);
}

BOOST_AUTO_TEST_CASE(hashtable_iterator_empty_table)
{
  HT t(8);
  BOOST_CHECK(t.begin() == t.end());
}

BOOST_AUTO_TEST_CASE(hashtable_const_iterator_visits_all_elements)
{
  HT t(8);
  t.add("a", 10);
  t.add("b", 20);
  const HT& ct = t;
  int sum = 0;
  for (auto it = ct.begin(); it != ct.end(); ++it)
  {
    sum += it->second;
  }
  BOOST_CHECK_EQUAL(sum, 30);
}

BOOST_AUTO_TEST_CASE(hashtable_iterator_count_matches_size)
{
  HT t(16);
  t.add("x", 1);
  t.add("y", 2);
  t.add("z", 3);
  std::size_t count = 0;
  for (auto it = t.begin(); it != t.end(); ++it)
  {
    ++count;
  }
  BOOST_CHECK_EQUAL(count, t.size());
}

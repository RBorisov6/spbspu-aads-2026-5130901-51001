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
  const int v = t.drop("x");
  BOOST_CHECK_EQUAL(v, 7);
  BOOST_CHECK(!t.has("x"));
  BOOST_CHECK_EQUAL(t.size(), 0u);
}

BOOST_AUTO_TEST_CASE(hashtable_drop_throws_for_missing_key)
{
  HT t(8);
  BOOST_CHECK_THROW(t.drop("ghost"), std::out_of_range);
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

BOOST_AUTO_TEST_CASE(hashtable_chains_grow_without_limit)
{
  HT t(1);
  t.add("aaa", 1);
  t.add("bbb", 2);
  BOOST_CHECK(t.has("aaa"));
  BOOST_CHECK(t.has("bbb"));
  BOOST_CHECK_EQUAL(t.size(), 2u);
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

BOOST_AUTO_TEST_CASE(hashtable_load_factor_empty)
{
  HT t(8);
  BOOST_CHECK_SMALL(t.loadFactor(), 1e-9);
}

BOOST_AUTO_TEST_CASE(hashtable_load_factor_after_inserts)
{
  HT t(8);
  t.add("a", 1);
  t.add("b", 2);
  BOOST_CHECK_CLOSE(t.loadFactor(), 2.0 / 8.0, 1e-6);
}

BOOST_AUTO_TEST_CASE(hashtable_longest_chain_empty)
{
  HT t(8);
  BOOST_CHECK_EQUAL(t.longestChain(), 0u);
}

BOOST_AUTO_TEST_CASE(hashtable_longest_chain_single_slot)
{
  HT t(1);
  t.add("a", 1);
  t.add("b", 2);
  t.add("c", 3);
  BOOST_CHECK_EQUAL(t.longestChain(), 3u);
}

BOOST_AUTO_TEST_CASE(hashtable_auto_rehash_on_load_factor)
{
  HT t(4);
  t.setMaxLoadFactor(0.5);
  t.add("a", 1);
  t.add("b", 2);
  // adding "c" would make lf = 3/4 = 0.75 > 0.5 → auto-rehash before insert
  t.add("c", 3);
  BOOST_CHECK(t.has("a"));
  BOOST_CHECK(t.has("b"));
  BOOST_CHECK(t.has("c"));
  BOOST_CHECK_EQUAL(t.size(), 3u);
  BOOST_CHECK_GT(t.slots(), 4u);
}

BOOST_AUTO_TEST_CASE(hashtable_auto_rehash_preserves_load_factor_below_limit)
{
  HT t(4);
  t.setMaxLoadFactor(0.5);
  for (int i = 0; i < 20; ++i)
  {
    t.add(std::to_string(i), i);
  }
  BOOST_CHECK_EQUAL(t.size(), 20u);
  BOOST_CHECK_LE(t.loadFactor(), 0.5 + 1e-9);
}

BOOST_AUTO_TEST_CASE(hashtable_auto_rehash_on_chain_length)
{
  HT t(4);
  t.setMaxChainLength(2);
  t.add("a", 1);
  t.add("b", 2);
  // if "c" would land in a bucket already at length 2, auto-rehash
  // after rehash all elements are redistributed, chain length check reapplied
  t.add("c", 3);
  t.add("d", 4);
  BOOST_CHECK(t.has("a"));
  BOOST_CHECK(t.has("b"));
  BOOST_CHECK(t.has("c"));
  BOOST_CHECK(t.has("d"));
  BOOST_CHECK_EQUAL(t.size(), 4u);
  BOOST_CHECK_LE(t.longestChain(), 2u);
}

BOOST_AUTO_TEST_CASE(hashtable_custom_resize_func)
{
  HT t(4);
  t.setMaxLoadFactor(0.5);
  t.setResizeFunc([](std::size_t n) { return n * 3 + 1; });
  t.add("a", 1);
  t.add("b", 2);
  t.add("c", 3);
  // rehash should have used n*3+1: 4*3+1=13
  BOOST_CHECK_EQUAL(t.slots(), 13u);
  BOOST_CHECK_EQUAL(t.size(), 3u);
}

BOOST_AUTO_TEST_CASE(hashtable_set_resize_func_preserved_in_copy)
{
  HT t(4);
  t.setMaxLoadFactor(0.5);
  t.setResizeFunc([](std::size_t n) { return n * 4; });
  HT t2(t);
  t2.add("a", 1);
  t2.add("b", 2);
  t2.add("c", 3);
  // t2 should use its copied resize func
  BOOST_CHECK_EQUAL(t2.size(), 3u);
  BOOST_CHECK(t2.has("a"));
}

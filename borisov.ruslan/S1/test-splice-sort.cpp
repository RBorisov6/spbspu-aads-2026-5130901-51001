#include <boost/test/unit_test.hpp>
#include "list.hpp"

namespace
{
  borisov::List< int > makeList(std::initializer_list< int > vals)
  {
    borisov::List< int > lst;
    for (const int v : vals)
    {
      lst.pushBack(v);
    }
    return lst;
  }

  bool listEquals(const borisov::List< int >& lst, std::initializer_list< int > vals)
  {
    borisov::List< int >::const_iterator it = lst.begin();
    for (const int v : vals)
    {
      if (it == lst.end() || *it != v)
      {
        return false;
      }
      ++it;
    }
    return it == lst.end();
  }
}

BOOST_AUTO_TEST_CASE(splice_all_into_empty)
{
  borisov::List< int > src = makeList({1, 2, 3});
  borisov::List< int > dst;
  dst.splice(dst.end(), src);
  BOOST_CHECK(src.empty());
  BOOST_CHECK(listEquals(dst, {1, 2, 3}));
}

BOOST_AUTO_TEST_CASE(splice_all_before_pos)
{
  borisov::List< int > src = makeList({1, 2, 3});
  borisov::List< int > dst = makeList({10, 20});
  borisov::List< int >::iterator pos = dst.begin();
  ++pos;
  dst.splice(pos, src);
  BOOST_CHECK(src.empty());
  BOOST_CHECK(listEquals(dst, {10, 1, 2, 3, 20}));
}

BOOST_AUTO_TEST_CASE(splice_single_element)
{
  borisov::List< int > src = makeList({1, 2, 3});
  borisov::List< int > dst = makeList({10, 20});
  borisov::List< int >::iterator it = src.begin();
  ++it;
  dst.splice(dst.end(), src, it);
  BOOST_CHECK_EQUAL(src.size(), 2u);
  BOOST_CHECK(listEquals(src, {1, 3}));
  BOOST_CHECK(listEquals(dst, {10, 20, 2}));
}

BOOST_AUTO_TEST_CASE(splice_range)
{
  borisov::List< int > src = makeList({1, 2, 3, 4, 5});
  borisov::List< int > dst = makeList({10, 20});
  borisov::List< int >::iterator first = src.begin();
  ++first;
  borisov::List< int >::iterator last = first;
  ++last;
  ++last;
  dst.splice(dst.begin(), src, first, last);
  BOOST_CHECK(listEquals(src, {1, 4, 5}));
  BOOST_CHECK(listEquals(dst, {2, 3, 10, 20}));
}

BOOST_AUTO_TEST_CASE(splice_from_end_of_src_is_noop)
{
  borisov::List< int > src = makeList({1, 2});
  borisov::List< int > dst = makeList({10});
  dst.splice(dst.end(), src, src.end());
  BOOST_CHECK(listEquals(src, {1, 2}));
  BOOST_CHECK(listEquals(dst, {10}));
}

BOOST_AUTO_TEST_CASE(sort_empty_list)
{
  borisov::List< int > lst;
  lst.sort();
  BOOST_CHECK(lst.empty());
}

BOOST_AUTO_TEST_CASE(sort_single_element)
{
  borisov::List< int > lst = makeList({42});
  lst.sort();
  BOOST_CHECK(listEquals(lst, {42}));
}

BOOST_AUTO_TEST_CASE(sort_already_sorted)
{
  borisov::List< int > lst = makeList({1, 2, 3, 4, 5});
  lst.sort();
  BOOST_CHECK(listEquals(lst, {1, 2, 3, 4, 5}));
}

BOOST_AUTO_TEST_CASE(sort_reverse_order)
{
  borisov::List< int > lst = makeList({5, 4, 3, 2, 1});
  lst.sort();
  BOOST_CHECK(listEquals(lst, {1, 2, 3, 4, 5}));
}

BOOST_AUTO_TEST_CASE(sort_arbitrary_order)
{
  borisov::List< int > lst = makeList({3, 1, 4, 1, 5, 9, 2, 6});
  lst.sort();
  BOOST_CHECK(listEquals(lst, {1, 1, 2, 3, 4, 5, 6, 9}));
}

BOOST_AUTO_TEST_CASE(sort_with_custom_comparator_descending)
{
  borisov::List< int > lst = makeList({3, 1, 4, 1, 5});
  lst.sort(std::greater< int >());
  BOOST_CHECK(listEquals(lst, {5, 4, 3, 1, 1}));
}

BOOST_AUTO_TEST_CASE(sort_preserves_size)
{
  borisov::List< int > lst = makeList({5, 3, 1, 4, 2});
  lst.sort();
  BOOST_CHECK_EQUAL(lst.size(), 5u);
}

BOOST_AUTO_TEST_CASE(merge_two_sorted_lists)
{
  borisov::List< int > a = makeList({1, 3, 5});
  borisov::List< int > b = makeList({2, 4, 6});
  a.merge(b);
  BOOST_CHECK(b.empty());
  BOOST_CHECK(listEquals(a, {1, 2, 3, 4, 5, 6}));
}

BOOST_AUTO_TEST_CASE(merge_with_empty_other)
{
  borisov::List< int > a = makeList({1, 2, 3});
  borisov::List< int > b;
  a.merge(b);
  BOOST_CHECK(b.empty());
  BOOST_CHECK(listEquals(a, {1, 2, 3}));
}

BOOST_AUTO_TEST_CASE(merge_into_empty_list)
{
  borisov::List< int > a;
  borisov::List< int > b = makeList({1, 2, 3});
  a.merge(b);
  BOOST_CHECK(b.empty());
  BOOST_CHECK(listEquals(a, {1, 2, 3}));
}

BOOST_AUTO_TEST_CASE(merge_with_duplicates)
{
  borisov::List< int > a = makeList({1, 3, 3});
  borisov::List< int > b = makeList({2, 3, 4});
  a.merge(b);
  BOOST_CHECK(b.empty());
  BOOST_CHECK(listEquals(a, {1, 2, 3, 3, 3, 4}));
}

BOOST_AUTO_TEST_CASE(merge_custom_comparator_descending)
{
  borisov::List< int > a = makeList({5, 3, 1});
  borisov::List< int > b = makeList({6, 4, 2});
  a.merge(b, std::greater< int >());
  BOOST_CHECK(b.empty());
  BOOST_CHECK(listEquals(a, {6, 5, 4, 3, 2, 1}));
}

BOOST_AUTO_TEST_CASE(partition_evens_and_odds)
{
  borisov::List< int > lst = makeList({1, 2, 3, 4, 5, 6});
  const borisov::List< int >::iterator mid = lst.partition(
    [](const int x)
    {
      return (x % 2) == 0;
    }
  );
  BOOST_CHECK_EQUAL(lst.size(), 6u);
  for (borisov::List< int >::iterator it = lst.begin(); it != mid; ++it)
  {
    BOOST_CHECK_EQUAL((*it) % 2, 0);
  }
  for (borisov::List< int >::iterator it = mid; it != lst.end(); ++it)
  {
    BOOST_CHECK_EQUAL((*it) % 2, 1);
  }
}

BOOST_AUTO_TEST_CASE(partition_all_satisfy)
{
  borisov::List< int > lst = makeList({2, 4, 6});
  const borisov::List< int >::iterator mid = lst.partition(
    [](const int x)
    {
      return (x % 2) == 0;
    }
  );
  BOOST_CHECK(mid == lst.end());
  BOOST_CHECK_EQUAL(lst.size(), 3u);
}

BOOST_AUTO_TEST_CASE(partition_none_satisfy)
{
  borisov::List< int > lst = makeList({1, 3, 5});
  const borisov::List< int >::iterator mid = lst.partition(
    [](const int x)
    {
      return (x % 2) == 0;
    }
  );
  BOOST_CHECK(mid == lst.begin());
  BOOST_CHECK_EQUAL(lst.size(), 3u);
}

BOOST_AUTO_TEST_CASE(partition_empty_list)
{
  borisov::List< int > lst;
  const borisov::List< int >::iterator mid = lst.partition(
    [](const int x)
    {
      return x > 0;
    }
  );
  BOOST_CHECK(mid == lst.end());
  BOOST_CHECK(lst.empty());
}

BOOST_AUTO_TEST_CASE(partition_preserves_relative_order)
{
  borisov::List< int > lst = makeList({3, 1, 4, 2, 5});
  const borisov::List< int >::iterator mid = lst.partition(
    [](const int x)
    {
      return x > 2;
    }
  );
  borisov::List< int >::iterator it = lst.begin();
  while (it != mid)
  {
    BOOST_CHECK_GT(*it, 2);
    ++it;
  }
  while (it != lst.end())
  {
    BOOST_CHECK_LE(*it, 2);
    ++it;
  }
}

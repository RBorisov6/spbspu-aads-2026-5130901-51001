#ifdef BUILD_TESTS
#define BOOST_TEST_MODULE ListTest
#include <boost/test/unit_test.hpp>
#include "list.hpp"


BOOST_AUTO_TEST_CASE(test_constructor_and_empty) {
  borisov::List<int> lst;
  BOOST_CHECK(lst.empty());
  BOOST_CHECK_EQUAL(lst.size(), 0u);
}

BOOST_AUTO_TEST_CASE(test_clear) {
  borisov::List<int> lst;
  lst.push_front(1);
  lst.push_front(2);
  lst.clear();
  BOOST_CHECK(lst.empty());
  BOOST_CHECK_EQUAL(lst.size(), 0u);
}

BOOST_AUTO_TEST_CASE(test_push_front) {
  borisov::List<int> lst;
  lst.push_front(10);
  BOOST_CHECK(!lst.empty());
  BOOST_CHECK_EQUAL(lst.size(), 1u);
  BOOST_CHECK_EQUAL(lst.front(), 10);
  BOOST_CHECK_EQUAL(lst.back(), 10);
}

BOOST_AUTO_TEST_CASE(test_push_back) {
  borisov::List<int> lst;
  lst.push_back(20);
  BOOST_CHECK(!lst.empty());
  BOOST_CHECK_EQUAL(lst.size(), 1u);
  BOOST_CHECK_EQUAL(lst.front(), 20);
  BOOST_CHECK_EQUAL(lst.back(), 20);
}

BOOST_AUTO_TEST_CASE(test_push_front_and_back) {
  borisov::List<int> lst;
  lst.push_front(1);
  lst.push_back(2);
  BOOST_CHECK_EQUAL(lst.size(), 2u);
  BOOST_CHECK_EQUAL(lst.front(), 1);
  BOOST_CHECK_EQUAL(lst.back(), 2);
}

BOOST_AUTO_TEST_CASE(test_pop_front) {
  borisov::List<int> lst;
  lst.push_back(10);
  lst.push_back(20);
  lst.pop_front();
  BOOST_CHECK_EQUAL(lst.size(), 1u);
  BOOST_CHECK_EQUAL(lst.front(), 20);
  BOOST_CHECK_EQUAL(lst.back(), 20);
  lst.pop_front();
  BOOST_CHECK(lst.empty());
}

BOOST_AUTO_TEST_CASE(test_pop_back) {
  borisov::List<int> lst;
  lst.push_front(10);
  lst.push_front(20);
  lst.pop_back();
  BOOST_CHECK_EQUAL(lst.size(), 1u);
  BOOST_CHECK_EQUAL(lst.front(), 20);
  BOOST_CHECK_EQUAL(lst.back(), 20);
  lst.pop_back();
  BOOST_CHECK(lst.empty());
}

BOOST_AUTO_TEST_CASE(test_iterator_forward) {
  borisov::List<int> lst;
  for (int i = 0; i < 5; ++i) {
    lst.push_back(i);
  }
  borisov::List<int>::iterator it = lst.begin();
  int expected = 0;
  while (it != lst.end()) {
    BOOST_CHECK_EQUAL(*it, expected);
    ++expected;
    ++it;
  }
  BOOST_CHECK_EQUAL(expected, 5);
}

BOOST_AUTO_TEST_CASE(test_iterator_backward) {
  borisov::List<int> lst;
  for (int i = 0; i < 5; ++i) {
    lst.push_back(i);
  }
  borisov::List<int>::iterator it = lst.end();
  --it;
  int expected = 4;
  while (true) {
    BOOST_CHECK_EQUAL(*it, expected);
    if (expected == 0) {
      break;
    }
    --expected;
    --it;
  }
}

BOOST_AUTO_TEST_CASE(test_const_iterator) {
  borisov::List<int> lst;
  lst.push_back(42);
  const borisov::List<int>& const_lst = lst;
  borisov::List<int>::const_iterator cit = const_lst.begin();
  BOOST_CHECK_EQUAL(*cit, 42);
  ++cit;
  BOOST_CHECK(cit == const_lst.end());
}

BOOST_AUTO_TEST_CASE(test_insert_begin) {
  borisov::List<int> lst;
  lst.push_back(10);
  borisov::List<int>::iterator it = lst.begin();
  lst.insert(it, 5);
  BOOST_CHECK_EQUAL(lst.size(), 2u);
  BOOST_CHECK_EQUAL(lst.front(), 5);
  BOOST_CHECK_EQUAL(lst.back(), 10);
}

BOOST_AUTO_TEST_CASE(test_insert_middle) {
  borisov::List<int> lst;
  lst.push_back(10);
  lst.push_back(30);
  borisov::List<int>::iterator it = lst.begin();
  ++it;
  lst.insert(it, 20);
  BOOST_CHECK_EQUAL(lst.size(), 3u);
  borisov::List<int>::iterator check = lst.begin();
  BOOST_CHECK_EQUAL(*check, 10);
  ++check;
  BOOST_CHECK_EQUAL(*check, 20);
  ++check;
  BOOST_CHECK_EQUAL(*check, 30);
}

BOOST_AUTO_TEST_CASE(test_insert_end) {
  borisov::List<int> lst;
  lst.push_back(10);
  borisov::List<int>::iterator it = lst.end();
  lst.insert(it, 20);
  BOOST_CHECK_EQUAL(lst.size(), 2u);
  BOOST_CHECK_EQUAL(lst.front(), 10);
  BOOST_CHECK_EQUAL(lst.back(), 20);
}

BOOST_AUTO_TEST_CASE(test_erase_begin) {
  borisov::List<int> lst;
  lst.push_back(10);
  lst.push_back(20);
  borisov::List<int>::iterator it = lst.begin();
  lst.erase(it);
  BOOST_CHECK_EQUAL(lst.size(), 1u);
  BOOST_CHECK_EQUAL(lst.front(), 20);
}

BOOST_AUTO_TEST_CASE(test_erase_middle) {
  borisov::List<int> lst;
  lst.push_back(10);
  lst.push_back(20);
  lst.push_back(30);
  borisov::List<int>::iterator it = lst.begin();
  ++it;
  lst.erase(it);
  BOOST_CHECK_EQUAL(lst.size(), 2u);
  borisov::List<int>::iterator check = lst.begin();
  BOOST_CHECK_EQUAL(*check, 10);
  ++check;
  BOOST_CHECK_EQUAL(*check, 30);
}

BOOST_AUTO_TEST_CASE(test_erase_end) {
  borisov::List<int> lst;
  lst.push_back(10);
  lst.push_back(20);
  borisov::List<int>::iterator it = lst.end();
  --it;
  lst.erase(it);
  BOOST_CHECK_EQUAL(lst.size(), 1u);
  BOOST_CHECK_EQUAL(lst.front(), 10);
}

BOOST_AUTO_TEST_CASE(test_copy_constructor) {
  borisov::List<int> original;
  original.push_back(1);
  original.push_back(2);
  borisov::List<int> copy(original);
  BOOST_CHECK_EQUAL(copy.size(), 2u);
  borisov::List<int>::iterator it = copy.begin();
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  copy.push_back(3);
  BOOST_CHECK_EQUAL(original.size(), 2u);
}

BOOST_AUTO_TEST_CASE(test_copy_assignment) {
  borisov::List<int> a;
  a.push_back(1);
  borisov::List<int> b;
  b = a;
  BOOST_CHECK_EQUAL(b.size(), 1u);
  BOOST_CHECK_EQUAL(b.front(), 1);
  a.push_back(2);
  BOOST_CHECK_EQUAL(b.size(), 1u);
}

BOOST_AUTO_TEST_CASE(test_move_constructor) {
  borisov::List<int> a;
  a.push_back(1);
  a.push_back(2);
  borisov::List<int> b(static_cast<borisov::List<int>&&>(a));
  BOOST_CHECK(a.empty());
  BOOST_CHECK_EQUAL(b.size(), 2u);
  borisov::List<int>::iterator it = b.begin();
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
}

BOOST_AUTO_TEST_CASE(test_move_assignment) {
  borisov::List<int> a;
  a.push_back(1);
  borisov::List<int> b;
  b = static_cast<borisov::List<int>&&>(a);
  BOOST_CHECK(a.empty());
  BOOST_CHECK_EQUAL(b.size(), 1u);
  BOOST_CHECK_EQUAL(b.front(), 1);
}


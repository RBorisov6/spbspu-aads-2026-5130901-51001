#define BOOST_TEST_MODULE ListTest
#include <boost/test/unit_test.hpp>
#include "list.hpp"

using namespace borisov;

BOOST_AUTO_TEST_CASE(test_constructor_and_empty) {
  List<int> lst;
  BOOST_CHECK(lst.empty());
  BOOST_CHECK_EQUAL(lst.size(), 0u);
}

BOOST_AUTO_TEST_CASE(test_clear) {
  List<int> lst;
  lst.push_front(1);
  lst.push_front(2);
  lst.clear();
  BOOST_CHECK(lst.empty());
  BOOST_CHECK_EQUAL(lst.size(), 0u);
}

BOOST_AUTO_TEST_CASE(test_push_front) {
  List<int> lst;
  lst.push_front(10);
  BOOST_CHECK(!lst.empty());
  BOOST_CHECK_EQUAL(lst.size(), 1u);
  BOOST_CHECK_EQUAL(lst.front(), 10);
  BOOST_CHECK_EQUAL(lst.back(), 10);
}

BOOST_AUTO_TEST_CASE(test_push_back) {
  List<int> lst;
  lst.push_back(20);
  BOOST_CHECK(!lst.empty());
  BOOST_CHECK_EQUAL(lst.size(), 1u);
  BOOST_CHECK_EQUAL(lst.front(), 20);
  BOOST_CHECK_EQUAL(lst.back(), 20);
}

BOOST_AUTO_TEST_CASE(test_push_front_and_back) {
  List<int> lst;
  lst.push_front(1);
  lst.push_back(2);
  BOOST_CHECK_EQUAL(lst.size(), 2u);
  BOOST_CHECK_EQUAL(lst.front(), 1);
  BOOST_CHECK_EQUAL(lst.back(), 2);
}

BOOST_AUTO_TEST_CASE(test_pop_front) {
  List<int> lst;
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
  List<int> lst;
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
  List<int> lst;
  for (int i = 0; i < 5; ++i) {
    lst.push_back(i);
  }
  List<int>::iterator it = lst.begin();
  int expected = 0;
  while (it != lst.end()) {
    BOOST_CHECK_EQUAL(*it, expected);
    ++expected;
    ++it;
  }
  BOOST_CHECK_EQUAL(expected, 5);
}

BOOST_AUTO_TEST_CASE(test_iterator_backward) {
  List<int> lst;
  for (int i = 0; i < 5; ++i) {
    lst.push_back(i);
  }
  List<int>::iterator it = lst.end();
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
  List<int> lst;
  lst.push_back(42);
  const List<int>& const_lst = lst;
  List<int>::const_iterator cit = const_lst.begin();
  BOOST_CHECK_EQUAL(*cit, 42);
  ++cit;
  BOOST_CHECK(cit == const_lst.end());
}

BOOST_AUTO_TEST_CASE(test_insert_begin) {
  List<int> lst;
  lst.push_back(10);
  List<int>::iterator it = lst.begin();
  lst.insert(it, 5);
  BOOST_CHECK_EQUAL(lst.size(), 2u);
  BOOST_CHECK_EQUAL(lst.front(), 5);
  BOOST_CHECK_EQUAL(lst.back(), 10);
}

BOOST_AUTO_TEST_CASE(test_insert_middle) {
  List<int> lst;
  lst.push_back(10);
  lst.push_back(30);
  List<int>::iterator it = lst.begin();
  ++it;
  lst.insert(it, 20);
  BOOST_CHECK_EQUAL(lst.size(), 3u);
  List<int>::iterator check = lst.begin();
  BOOST_CHECK_EQUAL(*check, 10);
  ++check;
  BOOST_CHECK_EQUAL(*check, 20);
  ++check;
  BOOST_CHECK_EQUAL(*check, 30);
}

BOOST_AUTO_TEST_CASE(test_insert_end) {
  List<int> lst;
  lst.push_back(10);
  List<int>::iterator it = lst.end();
  lst.insert(it, 20);
  BOOST_CHECK_EQUAL(lst.size(), 2u);
  BOOST_CHECK_EQUAL(lst.front(), 10);
  BOOST_CHECK_EQUAL(lst.back(), 20);
}

BOOST_AUTO_TEST_CASE(test_erase_begin) {
  List<int> lst;
  lst.push_back(10);
  lst.push_back(20);
  List<int>::iterator it = lst.begin();
  lst.erase(it);
  BOOST_CHECK_EQUAL(lst.size(), 1u);
  BOOST_CHECK_EQUAL(lst.front(), 20);
}

BOOST_AUTO_TEST_CASE(test_erase_middle) {
  List<int> lst;
  lst.push_back(10);
  lst.push_back(20);
  lst.push_back(30);
  List<int>::iterator it = lst.begin();
  ++it;
  lst.erase(it);
  BOOST_CHECK_EQUAL(lst.size(), 2u);
  List<int>::iterator check = lst.begin();
  BOOST_CHECK_EQUAL(*check, 10);
  ++check;
  BOOST_CHECK_EQUAL(*check, 30);
}

BOOST_AUTO_TEST_CASE(test_erase_end) {
  List<int> lst;
  lst.push_back(10);
  lst.push_back(20);
  List<int>::iterator it = lst.end();
  --it;
  lst.erase(it);
  BOOST_CHECK_EQUAL(lst.size(), 1u);
  BOOST_CHECK_EQUAL(lst.front(), 10);
}

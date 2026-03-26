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

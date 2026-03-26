#define BOOST_TEST_MODULE ListTest
#include <boost/test/unit_test.hpp>
#include "list.hpp"

using namespace borisov;

BOOST_AUTO_TEST_CASE(test_constructor_and_empty) {
  List<int> lst;
  BOOST_CHECK(lst.empty());
  BOOST_CHECK_EQUAL(lst.size(), 0u);
}


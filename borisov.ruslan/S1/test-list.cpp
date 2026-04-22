#define BOOST_TEST_MODULE ListTests
#include <boost/test/included/unit_test.hpp>
#include "list.hpp"

BOOST_AUTO_TEST_SUITE(list_tests)

BOOST_AUTO_TEST_CASE(empty_list_test) {
    borisov::List<int> list;
    BOOST_TEST(list.empty());
    BOOST_TEST(list.size() == 0);
}

BOOST_AUTO_TEST_CASE(push_front_test) {
    borisov::List<int> list;
    list.push_front(5);
    BOOST_TEST(!list.empty());
    BOOST_TEST(list.size() == 1);
    BOOST_TEST(list.front() == 5);
    BOOST_TEST(list.back() == 5);
}

BOOST_AUTO_TEST_CASE(push_back_test) {
    borisov::List<int> list;
    list.push_back(10);
    list.push_back(20);
    BOOST_TEST(list.size() == 2);
    BOOST_TEST(list.front() == 10);
    BOOST_TEST(list.back() == 20);
}

BOOST_AUTO_TEST_CASE(pop_front_test) {
    borisov::List<int> list;
    list.push_front(5);
    list.pop_front();
    BOOST_TEST(list.empty());
}

BOOST_AUTO_TEST_CASE(pop_back_test) {
    borisov::List<int> list;
    list.push_back(5);
    list.push_back(10);
    list.pop_back();
    BOOST_TEST(list.size() == 1);
    BOOST_TEST(list.back() == 5);
}

BOOST_AUTO_TEST_CASE(clear_test) {
    borisov::List<int> list;
    list.push_front(1);
    list.push_front(2);
    list.clear();
    BOOST_TEST(list.empty());
    BOOST_TEST(list.size() == 0);
}

BOOST_AUTO_TEST_CASE(iterator_forward_test) {
    borisov::List<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    borisov::List<int>::iterator it = list.begin();
    BOOST_TEST(*it == 1);
    ++it;
    BOOST_TEST(*it == 2);
    ++it;
    BOOST_TEST(*it == 3);
    ++it;
    BOOST_TEST(it == list.end());
}

BOOST_AUTO_TEST_CASE(const_iterator_test) {
    borisov::List<int> list;
    list.push_back(42);
    const borisov::List<int>& const_list = list;
    borisov::List<int>::const_iterator cit = const_list.begin();
    BOOST_TEST(*cit == 42);
    ++cit;
    BOOST_TEST(cit == const_list.end());
}

BOOST_AUTO_TEST_CASE(insert_test) {
    borisov::List<int> list;
    list.push_back(1);
    list.push_back(3);
    borisov::List<int>::iterator it = list.begin();
    ++it;
    list.insert(it, 2);
    borisov::List<int>::iterator check = list.begin();
    BOOST_TEST(*check == 1);
    ++check;
    BOOST_TEST(*check == 2);
    ++check;
    BOOST_TEST(*check == 3);
}

BOOST_AUTO_TEST_CASE(erase_test) {
    borisov::List<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    borisov::List<int>::iterator it = list.begin();
    ++it;
    list.erase(it);
    borisov::List<int>::iterator check = list.begin();
    BOOST_TEST(*check == 1);
    ++check;
    BOOST_TEST(*check == 3);
}

BOOST_AUTO_TEST_CASE(copy_constructor_test) {
    borisov::List<int> original;
    original.push_back(1);
    original.push_back(2);
    borisov::List<int> copy(original);
    BOOST_TEST(copy.size() == 2);
    borisov::List<int>::iterator it = copy.begin();
    BOOST_TEST(*it == 1);
    ++it;
    BOOST_TEST(*it == 2);
}

BOOST_AUTO_TEST_CASE(move_constructor_test) {
    borisov::List<int> a;
    a.push_back(1);
    a.push_back(2);
    borisov::List<int> b(std::move(a));
    BOOST_TEST(a.empty());
    BOOST_TEST(b.size() == 2);
    borisov::List<int>::iterator it = b.begin();
    BOOST_TEST(*it == 1);
    ++it;
    BOOST_TEST(*it == 2);
}

BOOST_AUTO_TEST_SUITE_END()

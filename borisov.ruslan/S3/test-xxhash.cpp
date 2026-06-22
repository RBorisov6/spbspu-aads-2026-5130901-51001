#include <boost/test/unit_test.hpp>
#include "xxhash.hpp"
#include <string>

BOOST_AUTO_TEST_CASE(xxhash32_empty_string_is_deterministic)
{
  borisov::XxHash32 h;
  std::size_t v1 = h(std::string(""));
  std::size_t v2 = h(std::string(""));
  BOOST_CHECK_EQUAL(v1, v2);
}

BOOST_AUTO_TEST_CASE(xxhash32_different_strings_give_different_hashes)
{
  borisov::XxHash32 h;
  BOOST_CHECK(h(std::string("abc")) != h(std::string("def")));
}

BOOST_AUTO_TEST_CASE(xxhash32_same_string_same_hash)
{
  borisov::XxHash32 h;
  std::string s = "hello world";
  BOOST_CHECK_EQUAL(h(s), h(s));
}

BOOST_AUTO_TEST_CASE(xxhash32_long_string_consistent)
{
  borisov::XxHash32 h;
  std::string s(64, 'x');
  BOOST_CHECK_EQUAL(h(s), h(s));
}

BOOST_AUTO_TEST_CASE(xxhash32_pair_of_strings)
{
  borisov::XxHash32 h;
  std::pair< std::string, std::string > p1("a", "b");
  std::pair< std::string, std::string > p2("a", "b");
  std::pair< std::string, std::string > p3("b", "a");
  BOOST_CHECK_EQUAL(h(p1), h(p2));
  BOOST_CHECK(h(p1) != h(p3));
}

BOOST_AUTO_TEST_CASE(xxhash32_seed_changes_output)
{
  uint32_t v0 = borisov::xxhash32("abc", 3, 0);
  uint32_t v1 = borisov::xxhash32("abc", 3, 1);
  BOOST_CHECK(v0 != v1);
}

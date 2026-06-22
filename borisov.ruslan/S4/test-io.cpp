#include <boost/test/unit_test.hpp>
#include "io.hpp"
#include <sstream>

BOOST_AUTO_TEST_CASE(io_read_single_dict)
{
  std::istringstream ss("first 1 name 2 surname\n");
  borisov::DataTable t;
  BOOST_CHECK(borisov::readDicts(ss, t));
  BOOST_CHECK(t.has("first"));
  BOOST_CHECK_EQUAL(t.get("first").get(1), "name");
  BOOST_CHECK_EQUAL(t.get("first").get(2), "surname");
}

BOOST_AUTO_TEST_CASE(io_read_multiple_dicts)
{
  std::istringstream ss(
    "first 1 name 2 surname\n"
    "second 4 mouse 1 name 2 keyboard\n"
  );
  borisov::DataTable t;
  BOOST_CHECK(borisov::readDicts(ss, t));
  BOOST_CHECK(t.has("first"));
  BOOST_CHECK(t.has("second"));
  BOOST_CHECK_EQUAL(t.get("second").get(4), "mouse");
}

BOOST_AUTO_TEST_CASE(io_empty_lines_ignored)
{
  std::istringstream ss("\nfirst 1 a\n\n");
  borisov::DataTable t;
  BOOST_CHECK(borisov::readDicts(ss, t));
  BOOST_CHECK(t.has("first"));
}

BOOST_AUTO_TEST_CASE(io_empty_dict)
{
  std::istringstream ss("empty\n");
  borisov::DataTable t;
  BOOST_CHECK(borisov::readDicts(ss, t));
  BOOST_CHECK(t.has("empty"));
  BOOST_CHECK(t.get("empty").empty());
}

BOOST_AUTO_TEST_CASE(io_sorted_order_preserved)
{
  std::istringstream ss("d 3 c 1 a 2 b\n");
  borisov::DataTable t;
  BOOST_CHECK(borisov::readDicts(ss, t));
  const borisov::Dict& dict = t.get("d");
  int prev = -1;
  for (auto it = dict.cbegin(); it != dict.cend(); ++it)
  {
    BOOST_CHECK(it->first > prev);
    prev = it->first;
  }
}

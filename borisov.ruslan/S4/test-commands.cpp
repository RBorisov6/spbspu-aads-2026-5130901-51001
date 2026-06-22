#include <boost/test/unit_test.hpp>
#include "commands.hpp"
#include "io.hpp"
#include <sstream>

namespace
{
  borisov::DataTable makeTable()
  {
    std::istringstream ss(
      "first 1 name 2 surname\n"
      "second 4 mouse 1 name 2 keyboard\n"
    );
    borisov::DataTable t;
    borisov::readDicts(ss, t);
    return t;
  }
}

BOOST_AUTO_TEST_CASE(cmd_print_basic)
{
  borisov::DataTable t = makeTable();
  std::ostringstream out;
  borisov::executeCommand("print first", t, out);
  BOOST_CHECK_EQUAL(out.str(), "first 1 name 2 surname\n");
}

BOOST_AUTO_TEST_CASE(cmd_print_sorted)
{
  borisov::DataTable t = makeTable();
  std::ostringstream out;
  borisov::executeCommand("print second", t, out);
  BOOST_CHECK_EQUAL(out.str(), "second 1 name 2 keyboard 4 mouse\n");
}

BOOST_AUTO_TEST_CASE(cmd_print_empty_dict)
{
  borisov::DataTable t;
  std::istringstream ss("empty\n");
  borisov::readDicts(ss, t);
  std::ostringstream out;
  borisov::executeCommand("print empty", t, out);
  BOOST_CHECK_EQUAL(out.str(), "<EMPTY>\n");
}

BOOST_AUTO_TEST_CASE(cmd_print_missing)
{
  borisov::DataTable t;
  std::ostringstream out;
  borisov::executeCommand("print noexist", t, out);
  BOOST_CHECK_EQUAL(out.str(), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_complement_basic)
{
  borisov::DataTable t = makeTable();
  std::ostringstream out;
  borisov::executeCommand("complement third second first", t, out);
  borisov::executeCommand("print third", t, out);
  BOOST_CHECK_EQUAL(out.str(), "third 4 mouse\n");
}

BOOST_AUTO_TEST_CASE(cmd_complement_missing_source)
{
  borisov::DataTable t;
  std::ostringstream out;
  borisov::executeCommand("complement r a b", t, out);
  BOOST_CHECK_EQUAL(out.str(), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_intersect_basic)
{
  borisov::DataTable t = makeTable();
  std::ostringstream out;
  borisov::executeCommand("intersect fourth first second", t, out);
  borisov::executeCommand("print fourth", t, out);
  BOOST_CHECK_EQUAL(out.str(), "fourth 1 name 2 surname\n");
}

BOOST_AUTO_TEST_CASE(cmd_intersect_left_value_wins)
{
  borisov::DataTable t = makeTable();
  std::ostringstream out;
  borisov::executeCommand("intersect yafourth second first", t, out);
  borisov::executeCommand("print yafourth", t, out);
  BOOST_CHECK_EQUAL(out.str(), "yafourth 1 name 2 keyboard\n");
}

BOOST_AUTO_TEST_CASE(cmd_union_basic)
{
  borisov::DataTable t = makeTable();
  std::ostringstream out;
  borisov::executeCommand("union fifth first second", t, out);
  borisov::executeCommand("print fifth", t, out);
  BOOST_CHECK_EQUAL(out.str(), "fifth 1 name 2 surname 4 mouse\n");
}

BOOST_AUTO_TEST_CASE(cmd_union_left_value_wins)
{
  borisov::DataTable t = makeTable();
  std::ostringstream out;
  borisov::executeCommand("union yafifth second first", t, out);
  borisov::executeCommand("print yafifth", t, out);
  BOOST_CHECK_EQUAL(out.str(), "yafifth 1 name 2 keyboard 4 mouse\n");
}

BOOST_AUTO_TEST_CASE(cmd_unknown)
{
  borisov::DataTable t;
  std::ostringstream out;
  borisov::executeCommand("badcmd", t, out);
  BOOST_CHECK_EQUAL(out.str(), "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(cmd_complement_result_already_exists)
{
  borisov::DataTable t = makeTable();
  std::ostringstream out;
  borisov::executeCommand("complement first second first", t, out);
  BOOST_CHECK_EQUAL(out.str(), "<INVALID COMMAND>\n");
}

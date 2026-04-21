#define BOOST_TEST_MODULE ProgramTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include "../run.hpp"

BOOST_AUTO_TEST_CASE(test_example_from_task) {
  std::istringstream input(
    "first 1 1 1\n"
    "second 2 2 2 2\n"
    "third\n"
    "fourth 4 4\n"
  );
  std::ostringstream output;
  std::ostringstream error;

  borisov::run(input, output, error);

  std::string expected =
    "first second third fourth\n"
    "1 2 4\n"
    "1 2 4\n"
    "1 2\n"
    "7 7 3 2\n";

  BOOST_CHECK_EQUAL(output.str(), expected);
}

#include <boost/test/unit_test.hpp>
#include "evaluateExpression.hpp"
#include <sstream>
#include <stdexcept>

BOOST_AUTO_TEST_CASE(valid_tokens_no_throw)
{
  std::ostringstream err;
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("1 + 2", err));
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("( 3 - 4 ) * 5", err));
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("! 0", err));
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("10 % 3", err));
}

BOOST_AUTO_TEST_CASE(invalid_token_throws)
{
  std::ostringstream err;
  BOOST_CHECK_THROW(borisov::evaluateExpression("1 + a", err), std::runtime_error);
  BOOST_CHECK_THROW(borisov::evaluateExpression("x * 2", err), std::runtime_error);
  BOOST_CHECK_THROW(borisov::evaluateExpression("5 $ 3", err), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(valid_infix_conversion_no_throw)
{
  std::ostringstream err;
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("1 + 2", err));
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("3 * 4 + 5", err));
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("( 1 + 2 ) * 3", err));
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("! 1", err));
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("2 + ! 3", err));
}

BOOST_AUTO_TEST_CASE(mismatched_parentheses_throws)
{
  std::ostringstream err;
  BOOST_CHECK_THROW(borisov::evaluateExpression("( 1 + 2", err), std::runtime_error);
  BOOST_CHECK_THROW(borisov::evaluateExpression("1 + 2 )", err), std::runtime_error);
  BOOST_CHECK_THROW(borisov::evaluateExpression(") 1 + 2 (", err), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(two_operands_in_row_throws)
{
  std::ostringstream err;
  BOOST_CHECK_THROW(borisov::evaluateExpression("1 2 + 3", err), std::runtime_error);
  BOOST_CHECK_THROW(borisov::evaluateExpression("+ 1 2", err), std::runtime_error);
  BOOST_CHECK_THROW(borisov::evaluateExpression("1 + + 2", err), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(simple_evaluation)
{
  std::ostringstream err;
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("1 + 2", err), 3);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("5 - 3", err), 2);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("4 * 7", err), 28);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("10 / 2", err), 5);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("10 % 3", err), 1);
}

BOOST_AUTO_TEST_CASE(unary_not_evaluation)
{
  std::ostringstream err;
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("! 0", err), ~0LL);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("! 1", err), ~1LL);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("3 + ! 1", err), 3 + ~1LL);
}

BOOST_AUTO_TEST_CASE(precedence_evaluation)
{
  std::ostringstream err;
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("2 + 3 * 4", err), 14);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("( 2 + 3 ) * 4", err), 20);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("! 1 + 2", err), (~1LL) + 2);
}

BOOST_AUTO_TEST_CASE(division_by_zero_throws)
{
  std::ostringstream err;
  BOOST_CHECK_THROW(borisov::evaluateExpression("1 / 0", err), std::runtime_error);
  BOOST_CHECK_THROW(borisov::evaluateExpression("5 % 0", err), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(leftover_operands_throws)
{
  std::ostringstream err;
  BOOST_CHECK_THROW(borisov::evaluateExpression("1 2", err), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(complex_expressions_from_task)
{
  std::ostringstream err;
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("( 1 + 2 ) * ( 3 - 4 )", err), -3);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("10 / ( 2 + 3 ) % 4", err), 2);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("4 * 7 - 3", err), 25);
}

BOOST_AUTO_TEST_CASE(unary_not_with_parentheses)
{
  std::ostringstream err;
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("! ( 1 + 2 )", err), ~3LL);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("( ! 1 ) + 5", err), (~1LL) + 5);
}

BOOST_AUTO_TEST_CASE(tokenizer_recognizes_not_operator)
{
  std::ostringstream err;
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("! 1", err));
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("2 + ! 3", err));
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("( ! 0 )", err));
}

BOOST_AUTO_TEST_CASE(tokenizer_rejects_invalid_not_position)
{
  std::ostringstream err;
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("1 !", err));
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("! 1 2", err));
}

BOOST_AUTO_TEST_CASE(infixToPostfix_handles_unary_not)
{
  std::ostringstream err;
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("! 1", err));
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("2 + ! 3", err));
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("! ( 1 + 2 )", err));
  BOOST_CHECK_NO_THROW(borisov::evaluateExpression("( ! 1 ) + 2", err));
}

BOOST_AUTO_TEST_CASE(infixToPostfix_rejects_invalid_not)
{
  std::ostringstream err;
  BOOST_CHECK_THROW(borisov::evaluateExpression("1 !", err), std::runtime_error);
  BOOST_CHECK_THROW(borisov::evaluateExpression("1 ! 2", err), std::runtime_error);
  BOOST_CHECK_THROW(borisov::evaluateExpression("! 1 !", err), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(eval_unary_not_positive)
{
  std::ostringstream err;
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("! 0", err), ~0LL);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("! 1", err), ~1LL);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("! -1", err), ~(-1LL));
}

BOOST_AUTO_TEST_CASE(eval_unary_not_with_binary_ops)
{
  std::ostringstream err;
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("3 + ! 1", err), 3 + ~1LL);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("! 5 * 2", err), (~5LL) * 2);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("( ! 2 ) + 4", err), (~2LL) + 4);
}

BOOST_AUTO_TEST_CASE(eval_unary_not_invalid_usage_throws)
{
  std::ostringstream err;
  BOOST_CHECK_THROW(borisov::evaluateExpression("! + 2", err), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(acceptance_mixed_operators)
{
  std::ostringstream err;
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("1 + 2 * 3 - 4 / 2", err), 1 + 2 * 3 - 4 / 2);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("10 % 3 + 5 * ( 2 - 1 )", err), 10 % 3 + 5 * (2 - 1));
}

BOOST_AUTO_TEST_CASE(acceptance_nested_parentheses)
{
  std::ostringstream err;
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("( ( 1 + 2 ) * ( 3 - 4 ) )", err), ((1 + 2) * (3 - 4)));
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("( 10 / ( 2 + 3 ) ) % 4", err), (10 / (2 + 3)) % 4);
}

BOOST_AUTO_TEST_CASE(acceptance_multiple_not)
{
  std::ostringstream err;
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("! ! 1", err), ~~1LL);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("! ( ! 0 )", err), ~(~0LL));
}

BOOST_AUTO_TEST_CASE(acceptance_negative_numbers)
{
  std::ostringstream err;
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("-1 + 2", err), 1);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("-5 * -3", err), 15);
}

BOOST_AUTO_TEST_CASE(acceptance_large_numbers)
{
  std::ostringstream err;
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("2147483647 + 1", err), 2147483647LL + 1);
  BOOST_CHECK_EQUAL(borisov::evaluateExpression("9223372036854775807 % 2", err), 9223372036854775807LL % 2);
}

BOOST_AUTO_TEST_CASE(acceptance_division_by_zero_errors)
{
  std::ostringstream err;
  BOOST_CHECK_THROW(borisov::evaluateExpression("1 / ( 2 - 2 )", err), std::runtime_error);
  BOOST_CHECK_THROW(borisov::evaluateExpression("5 % ( 3 / 3 - 1 )", err), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(acceptance_invalid_syntax_errors)
{
  std::ostringstream err;
  BOOST_CHECK_THROW(borisov::evaluateExpression("( 1 + 2", err), std::runtime_error);
  BOOST_CHECK_THROW(borisov::evaluateExpression("1 + + 2", err), std::runtime_error);
  BOOST_CHECK_THROW(borisov::evaluateExpression("1 2 3", err), std::runtime_error);
  BOOST_CHECK_THROW(borisov::evaluateExpression("+", err), std::runtime_error);
}

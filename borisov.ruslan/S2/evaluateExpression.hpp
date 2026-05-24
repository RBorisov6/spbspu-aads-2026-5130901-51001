#ifndef BORISOV_EVALUATE_EXPRESSION_HPP
#define BORISOV_EVALUATE_EXPRESSION_HPP

#include <iosfwd>
#include <string>

namespace borisov
{
  long long evaluateExpression(const std::string& expression, std::ostream& err);
}

#endif

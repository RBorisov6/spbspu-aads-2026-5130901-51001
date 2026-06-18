#include "evaluateExpression.hpp"
#include "stack.hpp"
#include "queue.hpp"
#include <string>
#include <sstream>
#include <cstdlib>
#include <cerrno>
#include <stdexcept>
#include <limits>

namespace borisov
{
  namespace
  {
    enum class TokenType
    {
      number,
      op_plus,
      op_minus,
      op_mult,
      op_div,
      op_mod,
      op_not,
      lparen,
      rparen
    };

    struct Token
    {
      TokenType type;
      long long value = 0;
    };

    int precedence(TokenType type)
    {
      if (type == TokenType::op_not)
      {
        return 3;
      }
      if (type == TokenType::op_mult || type == TokenType::op_div || type == TokenType::op_mod)
      {
        return 2;
      }
      if (type == TokenType::op_plus || type == TokenType::op_minus)
      {
        return 1;
      }
      return 0;
    }

    void tokenize(const std::string& expr, Queue< Token >& tokens, std::string& errorMsg)
    {
      std::istringstream iss(expr);
      std::string lex;
      while (iss >> lex)
      {
        if (lex == "+")
        {
          tokens.push(Token{TokenType::op_plus});
        }
        else if (lex == "-")
        {
          tokens.push(Token{TokenType::op_minus});
        }
        else if (lex == "*")
        {
          tokens.push(Token{TokenType::op_mult});
        }
        else if (lex == "/")
        {
          tokens.push(Token{TokenType::op_div});
        }
        else if (lex == "%")
        {
          tokens.push(Token{TokenType::op_mod});
        }
        else if (lex == "!")
        {
          tokens.push(Token{TokenType::op_not});
        }
        else if (lex == "(")
        {
          tokens.push(Token{TokenType::lparen});
        }
        else if (lex == ")")
        {
          tokens.push(Token{TokenType::rparen});
        }
        else
        {
          char* end = nullptr;
          errno = 0;
          long long val = std::strtoll(lex.c_str(), &end, 10);
          if (errno == ERANGE)
          {
            errorMsg = "Number out of range: " + lex;
            return;
          }
          if (*end != '\0')
          {
            errorMsg = "Invalid token: " + lex;
            return;
          }
          tokens.push(Token{TokenType::number, val});
        }
      }
    }

    bool infixToPostfix(Queue< Token >& infix, Queue< Token >& postfix, std::string& errorMsg)
    {
      Stack< Token > opStack;
      bool prevWasOperand = false;

      while (!infix.empty())
      {
        Token t = infix.front();
        infix.pop();

        if (t.type == TokenType::number)
        {
          if (prevWasOperand)
          {
            errorMsg = "Two operands in a row";
            return false;
          }
          postfix.push(t);
          prevWasOperand = true;
        }
        else if (t.type == TokenType::lparen)
        {
          opStack.push(t);
          prevWasOperand = false;
        }
        else if (t.type == TokenType::rparen)
        {
          while (!opStack.empty() && opStack.top().type != TokenType::lparen)
          {
            postfix.push(opStack.top());
            opStack.pop();
          }
          if (opStack.empty())
          {
            errorMsg = "Mismatched parentheses";
            return false;
          }
          opStack.pop();
          prevWasOperand = true;
        }
        else if (t.type == TokenType::op_not)
        {
          if (prevWasOperand)
          {
            errorMsg = "Operator ! cannot appear after an operand";
            return false;
          }
          opStack.push(t);
          prevWasOperand = false;
        }
        else
        {
          if (!prevWasOperand)
          {
            errorMsg = "Expected operand before binary operator";
            return false;
          }
          while (!opStack.empty() && opStack.top().type != TokenType::lparen &&
                 (precedence(opStack.top().type) > precedence(t.type) ||
                  (precedence(opStack.top().type) == precedence(t.type) && t.type != TokenType::op_not)))
          {
            postfix.push(opStack.top());
            opStack.pop();
          }
          opStack.push(t);
          prevWasOperand = false;
        }
      }

      while (!opStack.empty())
      {
        if (opStack.top().type == TokenType::lparen)
        {
          errorMsg = "Mismatched parentheses";
          return false;
        }
        postfix.push(opStack.top());
        opStack.pop();
      }
      return true;
    }

    long long safeAdd(long long a, long long b)
    {
      const long long maxVal = std::numeric_limits<long long>::max();
      const long long minVal = std::numeric_limits<long long>::min();
      if (b > 0 && a > maxVal - b)
      {
        throw std::overflow_error("overflow");
      }
      if (b < 0 && a < minVal - b)
      {
        throw std::overflow_error("overflow");
      }
      return a + b;
    }

    long long safeSub(long long a, long long b)
    {
      const long long maxVal = std::numeric_limits<long long>::max();
      const long long minVal = std::numeric_limits<long long>::min();
      if (b > 0 && a < minVal + b)
      {
        throw std::overflow_error("overflow");
      }
      if (b < 0 && a > maxVal + b)
      {
        throw std::overflow_error("overflow");
      }
      return a - b;
    }

    long long safeMul(long long a, long long b)
    {
      const long long maxVal = std::numeric_limits<long long>::max();
      const long long minVal = std::numeric_limits<long long>::min();
      if (a == 0 || b == 0)
      {
        return 0;
      }
      if ((a == -1 && b == minVal) || (b == -1 && a == minVal))
      {
        throw std::overflow_error("overflow");
      }
      if (a > 0)
      {
        if (b > 0)
        {
          if (a > maxVal / b)
          {
            throw std::overflow_error("overflow");
          }
        }
        else if (b < minVal / a)
        {
          throw std::overflow_error("overflow");
        }
      }
      else
      {
        if (b > 0)
        {
          if (a < minVal / b)
          {
            throw std::overflow_error("overflow");
          }
        }
        else if (a < maxVal / b)
        {
          throw std::overflow_error("overflow");
        }
      }
      return a * b;
    }

    long long safeDiv(long long a, long long b)
    {
      if (b == 0)
      {
        throw std::logic_error("division by zero");
      }
      if (a == std::numeric_limits<long long>::min() && b == -1)
      {
        throw std::overflow_error("overflow");
      }
      return a / b;
    }

    long long safeMod(long long a, long long b)
    {
      if (b == 0)
      {
        throw std::logic_error("division by zero");
      }
      if (a == std::numeric_limits<long long>::min() && b == -1)
      {
        return 0;
      }
      long long remainder = a % b;
      if (remainder >= 0)
      {
        return remainder;
      }
      long long divisorMagnitude = (b > 0) ? b : -static_cast<long long>(b);
      long long remainderMagnitude = (remainder > 0) ? remainder : -remainder;
      if (b > 0)
      {
        return divisorMagnitude - remainderMagnitude;
      }
      else
      {
        return -(divisorMagnitude - remainderMagnitude);
      }
    }

    long long evalPostfix(Queue< Token >& postfix, std::string& errorMsg)
    {
      Stack< long long > operands;
      while (!postfix.empty())
      {
        Token t = postfix.front();
        postfix.pop();

        if (t.type == TokenType::number)
        {
          operands.push(t.value);
        }
        else if (t.type == TokenType::op_not)
        {
          if (operands.empty())
          {
            errorMsg = "Not enough operands for !";
            return 0;
          }
          long long a = operands.top();
          operands.pop();
          operands.push(~a);
        }
        else
        {
          if (operands.size() < 2)
          {
            errorMsg = "Not enough operands for binary operator";
            return 0;
          }
          long long b = operands.top();
          operands.pop();
          long long a = operands.top();
          operands.pop();

          try
          {
            switch (t.type)
            {
            case TokenType::op_plus:
              operands.push(safeAdd(a, b));
              break;
            case TokenType::op_minus:
              operands.push(safeSub(a, b));
              break;
            case TokenType::op_mult:
              operands.push(safeMul(a, b));
              break;
            case TokenType::op_div:
              operands.push(safeDiv(a, b));
              break;
            case TokenType::op_mod:
              operands.push(safeMod(a, b));
              break;
            default:
              errorMsg = "Unknown operator";
              return 0;
            }
          }
          catch (const std::overflow_error&)
          {
            errorMsg = "Arithmetic overflow";
            return 0;
          }
          catch (const std::logic_error& e)
          {
            errorMsg = e.what();
            return 0;
          }
        }
      }

      if (operands.size() != 1)
      {
        errorMsg = "Invalid expression: leftover operands";
        return 0;
      }
      return operands.top();
    }
  }

  long long evaluateExpression(const std::string& expression, std::ostream& err)
  {
    Queue< Token > infixTokens;
    std::string errorMsg;

    tokenize(expression, infixTokens, errorMsg);
    if (!errorMsg.empty())
    {
      err << "Error: " << errorMsg;
      throw std::runtime_error("Tokenization failed");
    }

    Queue< Token > postfixTokens;
    if (!infixToPostfix(infixTokens, postfixTokens, errorMsg))
    {
      err << "Error: " << errorMsg;
      throw std::runtime_error("Conversion failed");
    }

    long long result = evalPostfix(postfixTokens, errorMsg);
    if (!errorMsg.empty())
    {
      err << "Error: " << errorMsg;
      throw std::runtime_error("Evaluation failed");
    }
    return result;
  }
}

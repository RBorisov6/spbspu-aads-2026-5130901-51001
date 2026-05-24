#include "evaluateExpression.hpp"
#include "stack.hpp"
#include "queue.hpp"
#include <string>
#include <sstream>
#include <cstdlib>
#include <stdexcept>

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
          long long val = std::strtoll(lex.c_str(), &end, 10);
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

          switch (t.type)
          {
          case TokenType::op_plus:
            operands.push(a + b);
            break;
          case TokenType::op_minus:
            operands.push(a - b);
            break;
          case TokenType::op_mult:
            operands.push(a * b);
            break;
          case TokenType::op_div:
            if (b == 0)
            {
              errorMsg = "Division by zero";
              return 0;
            }
            operands.push(a / b);
            break;
          case TokenType::op_mod:
            if (b == 0)
            {
              errorMsg = "Modulo by zero";
              return 0;
            }
            operands.push(a % b);
            break;
          default:
            errorMsg = "Unknown operator";
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

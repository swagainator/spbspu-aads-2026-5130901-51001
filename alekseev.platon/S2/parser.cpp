#include "parser.hpp"

#include <climits>
#include <stdexcept>

#include "operation.hpp"
#include <stack.hpp>

namespace alekseev
{
  namespace
  {
    bool isOperatorToken(const std::string& token)
    {
      return token.size() == 1 && isOperatorChar(token[0]);
    }

    long long parseNumberToken(const std::string& token)
    {
      if (token.empty())
      {
        throw std::runtime_error("invalid number token");
      }

      std::size_t i = 0;
      bool negative = false;
      if (token[0] == '-')
      {
        negative = true;
        i = 1;
      }

      if (i >= token.size())
      {
        throw std::runtime_error("invalid number token");
      }

      unsigned long long mag = 0;
      for (; i < token.size(); ++i)
      {
        const char c = token[i];
        if (c < '0' || c > '9')
        {
          throw std::runtime_error("invalid number token");
        }
        const unsigned digit = static_cast< unsigned >(c - '0');

        const unsigned long long limit = static_cast< unsigned long long >(LLONG_MAX) + 1ULL;
        if (mag > limit / 10ULL)
        {
          throw std::overflow_error("number overflow");
        }
        mag *= 10ULL;
        if (mag > limit - static_cast< unsigned long long >(digit))
        {
          throw std::overflow_error("number overflow");
        }
        mag += static_cast< unsigned long long >(digit);
      }

      if (!negative)
      {
        if (mag > static_cast< unsigned long long >(LLONG_MAX))
        {
          throw std::overflow_error("number overflow");
        }
        return static_cast< long long >(mag);
      }

      if (mag == static_cast< unsigned long long >(LLONG_MAX) + 1ULL)
      {
        return LLONG_MIN;
      }
      return -static_cast< long long >(mag);
    }

    Token parseToken(const std::string& token)
    {
      if (token == "(")
      {
        return Token::makeLParen();
      }
      if (token == ")")
      {
        return Token::makeRParen();
      }
      if (isOperatorToken(token))
      {
        return Token::makeOp(token[0]);
      }
      return Token::makeNumber(parseNumberToken(token));
    }

    void pushOperator(Stack< char >& ops, Queue< Token >& out, char op)
    {
      const int p = precedence(op);
      while (!ops.empty())
      {
        const char top = ops.top();
        if (top == '(')
        {
          break;
        }
        const int tp = precedence(top);
        const bool popByLeftAssoc = !isRightAssociative(op) && (tp >= p);
        const bool popByRightAssoc = isRightAssociative(op) && (tp > p);
        if (!popByLeftAssoc && !popByRightAssoc)
        {
          break;
        }
        out.push(Token::makeOp(ops.drop()));
      }
      ops.push(op);
    }

    void flushToken(const std::string& current, Stack< char >& ops, Queue< Token >& out)
    {
      if (current.empty())
      {
        throw std::runtime_error("empty token");
      }

      Token t = parseToken(current);
      if (t.type == TokenType::number)
      {
        out.push(t);
        return;
      }
      if (t.type == TokenType::lparen)
      {
        ops.push('(');
        return;
      }
      if (t.type == TokenType::rparen)
      {
        bool found = false;
        while (!ops.empty())
        {
          const char c = ops.drop();
          if (c == '(')
          {
            found = true;
            break;
          }
          out.push(Token::makeOp(c));
        }
        if (!found)
        {
          throw std::runtime_error("mismatched parentheses");
        }
        return;
      }
      if (t.type == TokenType::op)
      {
        pushOperator(ops, out, t.op);
        return;
      }

      throw std::runtime_error("unknown token type");
    }

    void requireStackSize(const Stack< long long >& st, std::size_t n)
    {
      if (st.size() < n)
      {
        throw std::runtime_error("not enough operands");
      }
    }

    void tokenizeAndFlush(const std::string& line, Stack< char >& ops, Queue< Token >& out)
    {
      std::string current;
      current.reserve(32);

      bool hadToken = false;
      for (std::size_t i = 0; i < line.size(); ++i)
      {
        const char c = line[i];
        if (c == ' ')
        {
          if (current.empty())
          {
            if (!hadToken && i + 1 == line.size())
            {
              return;
            }
            throw std::runtime_error("empty token");
          }
          flushToken(current, ops, out);
          current.clear();
          hadToken = true;
        }
        else
        {
          current.push_back(c);
        }
      }

      if (!current.empty())
      {
        flushToken(current, ops, out);
        hadToken = true;
      }

      if (!hadToken)
      {
        throw std::runtime_error("empty expression");
      }
    }
  }

  Queue< Token > infixToPostfix(const std::string& line)
  {
    Queue< Token > output;
    Stack< char > ops;

    tokenizeAndFlush(line, ops, output);

    while (!ops.empty())
    {
      const char c = ops.drop();
      if (c == '(')
      {
        throw std::runtime_error("mismatched parentheses");
      }
      output.push(Token::makeOp(c));
    }

    return output;
  }

  long long evalPostfix(Queue< Token > postfix)
  {
    Stack< long long > values;

    while (!postfix.empty())
    {
      const Token t = postfix.drop();
      if (t.type == TokenType::number)
      {
        values.push(t.value);
        continue;
      }
      if (t.type != TokenType::op)
      {
        throw std::runtime_error("unexpected token in postfix");
      }

      if (isUnary(t.op))
      {
        requireStackSize(values, 1);
        const long long a = values.drop();
        values.push(applyUnary(t.op, a));
      }
      else
      {
        requireStackSize(values, 2);
        const long long b = values.drop();
        const long long a = values.drop();
        values.push(applyBinary(t.op, a, b));
      }
    }

    if (values.size() != 1)
    {
      throw std::runtime_error("invalid expression");
    }

    return values.drop();
  }
}

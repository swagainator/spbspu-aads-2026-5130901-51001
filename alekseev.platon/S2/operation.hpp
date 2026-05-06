#ifndef ALEKSEEV_S2_OPERATION_HPP
#define ALEKSEEV_S2_OPERATION_HPP

#include <climits>
#include <stdexcept>

namespace alekseev
{
  inline int precedence(char op)
  {
    if (op == '#')
    {
      return 3;
    }
    if (op == '*' || op == '/' || op == '%')
    {
      return 2;
    }
    if (op == '+' || op == '-')
    {
      return 1;
    }
    throw std::logic_error("unknown operator");
  }

  inline bool isRightAssociative(char op)
  {
    return op == '#';
  }

  inline bool isUnary(char op)
  {
    return op == '#';
  }

  inline long long applyBinary(char op, long long a, long long b)
  {
    long long res = 0;
    if (op == '+')
    {
      if (__builtin_add_overflow(a, b, &res))
      {
        throw std::overflow_error("addition overflow");
      }
      return res;
    }
    if (op == '-')
    {
      if (__builtin_sub_overflow(a, b, &res))
      {
        throw std::overflow_error("subtraction overflow");
      }
      return res;
    }
    if (op == '*')
    {
      if (__builtin_mul_overflow(a, b, &res))
      {
        throw std::overflow_error("multiplication overflow");
      }
      return res;
    }
    if (op == '/')
    {
      if (b == 0)
      {
        throw std::domain_error("division by zero");
      }
      if (a == LLONG_MIN && b == -1)
      {
        throw std::overflow_error("division overflow");
      }
      return a / b;
    }
    if (op == '%')
    {
      if (b == 0)
      {
        throw std::domain_error("modulo by zero");
      }
      res = a % b;
      if (res < 0)
      {
        res += std::abs(b);
      }
      return res;
    }
    throw std::logic_error("unknown binary operator");
  }

  inline long long reverseNumber(long long x)
  {
    const bool negative = x < 0;
    unsigned long long mag = 0;
    if (negative)
    {
      if (x == LLONG_MIN)
      {
        mag = static_cast< unsigned long long >(LLONG_MAX) + 1ULL;
      }
      else
      {
        mag = static_cast< unsigned long long >(-x);
      }
    }
    else
    {
      mag = static_cast< unsigned long long >(x);
    }

    const unsigned long long limit = negative ?
      (static_cast< unsigned long long >(LLONG_MAX) + 1ULL) :
      static_cast< unsigned long long >(LLONG_MAX);

    unsigned long long rev = 0;
    while (mag > 0)
    {
      const unsigned digit = static_cast< unsigned >(mag % 10ULL);
      if (rev > limit / 10ULL)
      {
        throw std::overflow_error("reverse overflow");
      }
      rev *= 10ULL;
      if (rev > limit - static_cast< unsigned long long >(digit))
      {
        throw std::overflow_error("reverse overflow");
      }
      rev += static_cast< unsigned long long >(digit);
      mag /= 10ULL;
    }

    if (!negative)
    {
      return static_cast< long long >(rev);
    }


    if (rev == static_cast< unsigned long long >(LLONG_MAX) + 1ULL)
    {
      return LLONG_MIN;
    }

    return -static_cast< long long >(rev);
  }

  inline long long applyUnary(char op, long long a)
  {
    if (op == '#')
    {
      return reverseNumber(a);
    }
    throw std::logic_error("unknown unary operator");
  }
}

#endif

#ifndef ALEKSEEV_S2_TOKEN_HPP
#define ALEKSEEV_S2_TOKEN_HPP


namespace alekseev
{
  enum class TokenType
  {
    number,
    lparen,
    rparen,
    op
  };

  struct Token
  {
    TokenType type;
    long long value;
    char op;

    Token() = delete;

    static Token makeNumber(long long v)
    {
      Token t{TokenType::number, v, 0};
      return t;
    }

    static Token makeLParen()
    {
      Token t{TokenType::lparen, 0, 0};
      return t;
    }

    static Token makeRParen()
    {
      Token t{TokenType::rparen, 0, 0};
      return t;
    }

    static Token makeOp(char c)
    {
      Token t{TokenType::op, 0, c};
      return t;
    }
  };

  inline bool isOperatorChar(char c)
  {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '#';
  }
}

#endif

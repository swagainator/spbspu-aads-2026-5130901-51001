#ifndef ALEKSEEV_S2_PARSER_HPP
#define ALEKSEEV_S2_PARSER_HPP

#include <string>

#include <queue.hpp>
#include "token.hpp"

namespace alekseev
{
  Queue< Token > infixToPostfix(const std::string& line);
  long long evalPostfix(Queue< Token > postfix);
}

#endif

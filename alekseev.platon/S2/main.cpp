#include <fstream>
#include <iostream>
#include <string>

#include "parser.hpp"
#include <stack.hpp>

namespace
{
  int evaluateAll(std::istream& input)
  {
    alekseev::Stack< long long > results;
    std::string line;

    while (std::getline(input, line))
    {
      if (line.empty())
      {
        continue;
      }

      const long long value = alekseev::evalPostfix(alekseev::infixToPostfix(line));
      results.push(value);
    }

    bool first = true;
    while (!results.empty())
    {
      if (!first)
      {
        std::cout << ' ';
      }
      first = false;
      std::cout << results.drop();
    }
    std::cout << '\n';

    return 0;
  }
}

int main(int argc, char* argv[])
{
  try
  {
    if (argc == 2)
    {
      std::ifstream file(argv[1]);
      if (!file)
      {
        std::cerr << "cannot open file\n";
        return 1;
      }
      return evaluateAll(file);
    }

    if (argc == 1)
    {
      return evaluateAll(std::cin);
    }

    std::cerr << "invalid arguments\n";
    return 1;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    return 1;
  }
}

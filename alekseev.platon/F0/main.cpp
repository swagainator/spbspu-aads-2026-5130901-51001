#include <exception>
#include <iostream>
#include <string>

#include "command_processor.hpp"
#include "project_storage.hpp"

int main(int argc, char*[])
{
  try
  {
    if (argc != 1)
    {
      std::cerr << "invalid arguments\n";
      return 1;
    }

    alekseev::ProjectStorage storage;
    alekseev::CommandProcessor processor(storage, std::cout);
    std::string line;
    while (std::getline(std::cin, line))
    {
      processor.processLine(line);
    }
    return 0;
  }
  catch (const std::exception& e)
  {
    std::cerr << "internal error: " << e.what() << '\n';
    return 2;
  }
  catch (...)
  {
    std::cerr << "internal error\n";
    return 2;
  }
}

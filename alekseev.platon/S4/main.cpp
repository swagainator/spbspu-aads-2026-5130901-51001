#include <exception>
#include <fstream>
#include <iostream>
#include <string>

#include "commands.hpp"
#include "dataset.hpp"

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "filename is required\n";
      return 1;
    }

    std::ifstream input(argv[1]);
    if (!input)
    {
      std::cerr << "cannot open file\n";
      return 1;
    }

    alekseev::DictionaryStorage storage;
    if (!alekseev::loadDatasets(input, storage))
    {
      std::cerr << "invalid file\n";
      return 1;
    }

    const alekseev::CommandTable commands = alekseev::makeCommandTable();
    std::string line;
    while (std::getline(std::cin, line))
    {
      alekseev::dispatchCommand(commands, line, storage, std::cout);
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

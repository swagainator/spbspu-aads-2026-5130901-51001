#include <boost/test/unit_test.hpp>

#include <sstream>
#include <string>

#include "commands.hpp"
#include "dataset.hpp"

namespace
{
  void runLines(std::istream& in, alekseev::DictionaryStorage& storage, std::ostream& out)
  {
    const alekseev::CommandTable commands = alekseev::makeCommandTable();
    std::string line;
    while (std::getline(in, line))
    {
      alekseev::dispatchCommand(commands, line, storage, out);
    }
  }
}

BOOST_AUTO_TEST_CASE(command_loop_continues)
{
  std::istringstream data("first 1 name 2 surname\nsecond 2 keyboard 3 mouse\n");
  alekseev::DictionaryStorage storage;
  BOOST_TEST(alekseev::loadDatasets(data, storage));

  std::istringstream commands(
      "print first\n"
      "bad command\n"
      "union third first second\n"
      "print third\n");
  std::ostringstream out;
  runLines(commands, storage, out);

  const std::string expected =
      "first 1 name 2 surname\n"
      "<INVALID COMMAND>\n"
      "third 1 name 2 surname 3 mouse\n";
  BOOST_TEST(out.str() == expected);
}

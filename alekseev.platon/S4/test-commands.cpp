#include <boost/test/unit_test.hpp>

#include <sstream>
#include <string>

#include "commands.hpp"

namespace
{
  std::string dispatch(const std::string& line, alekseev::DictionaryStorage& storage)
  {
    const alekseev::CommandTable commands = alekseev::makeCommandTable();
    std::ostringstream out;
    alekseev::dispatchCommand(commands, line, storage, out);
    return out.str();
  }

  alekseev::Dictionary makeFirst()
  {
    alekseev::Dictionary dictionary;
    dictionary.push(1, "name");
    dictionary.push(2, "surname");
    return dictionary;
  }

  alekseev::Dictionary makeSecond()
  {
    alekseev::Dictionary dictionary;
    dictionary.push(4, "mouse");
    dictionary.push(1, "name");
    dictionary.push(2, "keyboard");
    return dictionary;
  }

  alekseev::Dictionary makeMachineSecond()
  {
    alekseev::Dictionary dictionary = makeSecond();
    dictionary.push(3, "machine");
    return dictionary;
  }
}

BOOST_AUTO_TEST_CASE(print_and_invalid_commands)
{
  const alekseev::CommandTable commands = alekseev::makeCommandTable();
  BOOST_TEST(commands.contains("print"));
  BOOST_TEST(commands.contains("complement"));
  BOOST_TEST(commands.contains("intersect"));
  BOOST_TEST(commands.contains("union"));
  BOOST_TEST(!commands.contains("merge"));

  alekseev::DictionaryStorage storage;
  storage.push("first", makeFirst());
  storage.push("empty", alekseev::Dictionary());
  BOOST_TEST(dispatch("print first", storage) == "first 1 name 2 surname\n");
  BOOST_TEST(dispatch("print empty", storage) == "<EMPTY>\n");
  BOOST_TEST(dispatch("print missing", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(dispatch("print first extra", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(dispatch("unknown", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(dispatch("print", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(dispatch("", storage).empty());
}

BOOST_AUTO_TEST_CASE(set_operations)
{
  alekseev::DictionaryStorage storage;
  storage.push("first", makeFirst());
  storage.push("second", makeSecond());

  BOOST_TEST(dispatch("complement third second first", storage).empty());
  BOOST_TEST(dispatch("print third", storage) == "third 4 mouse\n");
  BOOST_TEST(dispatch("complement third second first", storage).empty());
  BOOST_TEST(dispatch("print third", storage) == "third 4 mouse\n");
  BOOST_TEST(dispatch("complement broken second", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(dispatch("complement broken missing first", storage) == "<INVALID COMMAND>\n");

  BOOST_TEST(dispatch("complement empty first second", storage).empty());
  BOOST_TEST(dispatch("print empty", storage) == "<EMPTY>\n");

  BOOST_TEST(dispatch("intersect fourth first second", storage).empty());
  BOOST_TEST(dispatch("print fourth", storage) == "fourth 1 name 2 surname\n");
  BOOST_TEST(dispatch("intersect bad first", storage) == "<INVALID COMMAND>\n");

  BOOST_TEST(dispatch("union fifth first second", storage).empty());
  BOOST_TEST(dispatch("print fifth", storage) == "fifth 1 name 2 surname 4 mouse\n");
  BOOST_TEST(dispatch("union bad first second extra", storage) == "<INVALID COMMAND>\n");

  BOOST_TEST(dispatch("bad command", storage) == "<INVALID COMMAND>\n");
  BOOST_TEST(dispatch("print fifth", storage) == "fifth 1 name 2 surname 4 mouse\n");
}

BOOST_AUTO_TEST_CASE(set_operation_overwrite_aliases)
{
  {
    alekseev::DictionaryStorage storage;
    storage.push("first", makeFirst());
    storage.push("second", makeSecond());

    BOOST_TEST(dispatch("complement second second first", storage).empty());
    BOOST_TEST(dispatch("print second", storage) == "second 4 mouse\n");
  }
  {
    alekseev::DictionaryStorage storage;
    storage.push("first", makeFirst());
    storage.push("second", makeSecond());

    BOOST_TEST(dispatch("intersect second second first", storage).empty());
    BOOST_TEST(dispatch("print second", storage) == "second 1 name 2 keyboard\n");
  }
  {
    alekseev::DictionaryStorage storage;
    storage.push("first", makeFirst());
    storage.push("second", makeMachineSecond());

    BOOST_TEST(dispatch("union first second first", storage).empty());
    BOOST_TEST(dispatch("print first", storage) ==
        "first 1 name 2 keyboard 3 machine 4 mouse\n");
  }
  {
    alekseev::DictionaryStorage storage;
    storage.push("first", makeFirst());
    storage.push("third", makeFirst());
    storage.push("fourth", alekseev::Dictionary());

    BOOST_TEST(dispatch("intersect first third fourth", storage).empty());
    BOOST_TEST(dispatch("print first", storage) == "<EMPTY>\n");
  }
}

BOOST_AUTO_TEST_CASE(acceptance_scenario)
{
  alekseev::DictionaryStorage storage;

  alekseev::Dictionary first;
  first.push(1, "name");
  first.push(2, "surname");
  storage.push("first", first);

  alekseev::Dictionary second;
  second.push(4, "mouse");
  second.push(1, "name");
  second.push(2, "keyboard");
  storage.push("second", second);

  std::string output;
  output += dispatch("print second", storage);
  output += dispatch("complement third second first", storage);
  output += dispatch("print third", storage);
  output += dispatch("intersect fourth first second", storage);
  output += dispatch("print fourth", storage);
  output += dispatch("union fifth first second", storage);
  output += dispatch("print fifth", storage);

  const std::string expected =
      "second 1 name 2 keyboard 4 mouse\n"
      "third 4 mouse\n"
      "fourth 1 name 2 surname\n"
      "fifth 1 name 2 surname 4 mouse\n";
  BOOST_TEST(output == expected);
}

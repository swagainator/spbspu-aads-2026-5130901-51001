#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <sstream>
#include <string>

#include "command_processor.hpp"
#include "project_storage.hpp"

namespace
{
  std::string runAdvancedCommands(const std::string& commands)
  {
    alekseev::ProjectStorage storage;
    std::ostringstream out;
    alekseev::CommandProcessor processor(storage, out);
    std::istringstream input(commands);
    std::string line;
    while (std::getline(input, line))
    {
      processor.processLine(line);
    }
    return out.str();
  }

  std::string buildOrderLine(std::size_t count)
  {
    std::string expected;
    for (std::size_t i = 0; i < count; ++i)
    {
      if (i != 0)
      {
        expected += ' ';
      }
      expected += "T" + std::to_string(i);
    }
    expected += '\n';
    return expected;
  }
}

BOOST_AUTO_TEST_CASE(command_advanced_single_project_flow)
{
  BOOST_CHECK_EQUAL(runAdvancedCommands(
      "make p\n"
      "add-task p A\n"
      "add-task p B\n"
      "add-dep p A B\n"
      "show-task p A\n"
      "ready p\n"
      "build-order p\n"
      "check-cycle p\n"),
      "<TASK: A, DEPENDS-ON: 1, REQUIRED-FOR: 0>\n"
      "B\n"
      "B A\n"
      "<CYCLE: NO>\n");
}

BOOST_AUTO_TEST_CASE(command_advanced_large_chain)
{
  const std::size_t count = 55;
  std::string commands("make p\n");
  for (std::size_t i = 0; i < count; ++i)
  {
    commands += "add-task p T" + std::to_string(i) + "\n";
  }
  for (std::size_t i = 1; i < count; ++i)
  {
    commands += "add-dep p T" + std::to_string(i) + " T" + std::to_string(i - 1) + "\n";
  }
  commands += "ready p\n";
  commands += "build-order p\n";

  BOOST_CHECK_EQUAL(runAdvancedCommands(commands), "T0\n" + buildOrderLine(count));
}

BOOST_AUTO_TEST_CASE(command_advanced_rename_delete_and_reinsert)
{
  BOOST_CHECK_EQUAL(runAdvancedCommands(
      "make p\n"
      "add-task p A\n"
      "add-task p B\n"
      "add-task p C\n"
      "add-dep p A B\n"
      "add-dep p C A\n"
      "rename-task p A X\n"
      "remove-task p B\n"
      "add-task p D\n"
      "add-dep p D X\n"
      "show-task p X\n"
      "ready p\n"
      "build-order p\n"),
      "<TASK: X, DEPENDS-ON: 0, REQUIRED-FOR: 2>\n"
      "X\n"
      "X C D\n");
}

BOOST_AUTO_TEST_CASE(command_advanced_cycle_detection)
{
  BOOST_CHECK_EQUAL(runAdvancedCommands(
      "make p\n"
      "add-task p A\n"
      "add-task p B\n"
      "add-task p C\n"
      "add-dep p A B\n"
      "add-dep p B C\n"
      "add-dep p C A\n"
      "check-cycle p\n"
      "build-order p\n"),
      "<CYCLE: YES>\n"
      "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(command_advanced_merge_projects)
{
  BOOST_CHECK_EQUAL(runAdvancedCommands(
      "make p1\n"
      "add-task p1 A\n"
      "add-task p1 B\n"
      "add-dep p1 B A\n"
      "make p2\n"
      "add-task p2 A\n"
      "add-task p2 C\n"
      "add-dep p2 C A\n"
      "merge p3 p1 p2\n"
      "show p3\n"
      "build-order p3\n"
      "check-cycle p3\n"),
      "<PROJECT: p3, TASKS: 3, DEPS: 2>\n"
      "A B C\n"
      "<CYCLE: NO>\n");
}

BOOST_AUTO_TEST_CASE(command_advanced_many_project_storage_operations)
{
  std::string commands;
  for (std::size_t i = 0; i < 45; ++i)
  {
    commands += "make P" + std::to_string(i) + "\n";
  }
  for (std::size_t i = 0; i < 15; ++i)
  {
    commands += "drop P" + std::to_string(i) + "\n";
  }
  for (std::size_t i = 0; i < 15; ++i)
  {
    commands += "make Q" + std::to_string(i) + "\n";
  }
  commands += "add-task P20 Build\n";
  commands += "add-task Q10 Build\n";
  commands += "show P20\n";
  commands += "check-project-dep P20 Q10\n";

  BOOST_CHECK_EQUAL(runAdvancedCommands(commands),
      "<PROJECT: P20, TASKS: 1, DEPS: 0>\n"
      "<PROJECT-DEP: NO>\n");
}

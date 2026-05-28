#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <sstream>
#include <string>

#include "command_processor.hpp"
#include "project_storage.hpp"

namespace
{
  std::string runCommands(const char* commands)
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
}

BOOST_AUTO_TEST_CASE(command_make_and_show)
{
  BOOST_CHECK_EQUAL(runCommands(
      "make p1\n"
      "show p1\n"),
      "<PROJECT: p1, TASKS: 0, DEPS: 0>\n");
}

BOOST_AUTO_TEST_CASE(command_duplicate_project)
{
  BOOST_CHECK_EQUAL(runCommands(
      "make p1\n"
      "make p1\n"),
      "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(command_task_flow)
{
  BOOST_CHECK_EQUAL(runCommands(
      "make p1\n"
      "add-task p1 compile\n"
      "add-task p1 build\n"
      "add-dep p1 build compile\n"
      "show p1\n"
      "show-task p1 build\n"
      "ready p1\n"),
      "<PROJECT: p1, TASKS: 2, DEPS: 1>\n"
      "<TASK: build, DEPENDS-ON: 1, REQUIRED-FOR: 0>\n"
      "compile\n");
}

BOOST_AUTO_TEST_CASE(command_remove_task_updates_project)
{
  BOOST_CHECK_EQUAL(runCommands(
      "make p1\n"
      "add-task p1 compile\n"
      "add-task p1 build\n"
      "add-dep p1 build compile\n"
      "remove-task p1 build\n"
      "show p1\n"
      "show-task p1 compile\n"),
      "<PROJECT: p1, TASKS: 1, DEPS: 0>\n"
      "<TASK: compile, DEPENDS-ON: 0, REQUIRED-FOR: 0>\n");
}

BOOST_AUTO_TEST_CASE(command_rename_task_preserves_edges)
{
  BOOST_CHECK_EQUAL(runCommands(
      "make p1\n"
      "add-task p1 compile\n"
      "add-task p1 build\n"
      "add-task p1 test\n"
      "add-dep p1 build compile\n"
      "add-dep p1 test build\n"
      "rename-task p1 build package\n"
      "show-task p1 package\n"
      "show-task p1 test\n"),
      "<TASK: package, DEPENDS-ON: 1, REQUIRED-FOR: 1>\n"
      "<TASK: test, DEPENDS-ON: 1, REQUIRED-FOR: 0>\n");
}

BOOST_AUTO_TEST_CASE(command_cycle_check)
{
  BOOST_CHECK_EQUAL(runCommands(
      "make p1\n"
      "add-task p1 a\n"
      "add-task p1 b\n"
      "add-dep p1 a b\n"
      "add-dep p1 b a\n"
      "check-cycle p1\n"),
      "<CYCLE: YES>\n");
}

BOOST_AUTO_TEST_CASE(command_build_order)
{
  BOOST_CHECK_EQUAL(runCommands(
      "make p1\n"
      "add-task p1 compile\n"
      "add-task p1 build\n"
      "add-task p1 test\n"
      "add-dep p1 build compile\n"
      "add-dep p1 test build\n"
      "build-order p1\n"),
      "compile build test\n");
}

BOOST_AUTO_TEST_CASE(command_ready_without_ready_tasks_prints_blank_line)
{
  BOOST_CHECK_EQUAL(runCommands(
      "make p1\n"
      "add-task p1 a\n"
      "add-task p1 b\n"
      "add-dep p1 a b\n"
      "add-dep p1 b a\n"
      "ready p1\n"),
      "\n");
}

BOOST_AUTO_TEST_CASE(command_merge)
{
  BOOST_CHECK_EQUAL(runCommands(
      "make p1\n"
      "add-task p1 compile\n"
      "add-task p1 build\n"
      "add-dep p1 build compile\n"
      "make p2\n"
      "add-task p2 test\n"
      "merge p3 p1 p2\n"
      "show p3\n"),
      "<PROJECT: p3, TASKS: 3, DEPS: 1>\n");
}

BOOST_AUTO_TEST_CASE(command_check_project_dependency)
{
  BOOST_CHECK_EQUAL(runCommands(
      "make p1\n"
      "add-task p1 compile\n"
      "add-task p1 build\n"
      "add-dep p1 build compile\n"
      "make p2\n"
      "add-task p2 compile\n"
      "check-project-dep p1 p2\n"),
      "<PROJECT-DEP: YES>\n");
}

BOOST_AUTO_TEST_CASE(command_check_project_dependency_without_path)
{
  BOOST_CHECK_EQUAL(runCommands(
      "make p1\n"
      "add-task p1 compile\n"
      "make p2\n"
      "add-task p2 compile\n"
      "check-project-dep p1 p2\n"),
      "<PROJECT-DEP: NO>\n");
}

BOOST_AUTO_TEST_CASE(command_show_project_dependencies)
{
  BOOST_CHECK_EQUAL(runCommands(
      "make p1\n"
      "add-task p1 compile\n"
      "add-task p1 build\n"
      "add-dep p1 build compile\n"
      "make p2\n"
      "add-task p2 compile\n"
      "show-project-deps p1\n"),
      "<PROJECT-DEPS: p1, COUNT: 1>\n"
      "p2\n");
}

BOOST_AUTO_TEST_CASE(command_show_project_dependencies_empty)
{
  BOOST_CHECK_EQUAL(runCommands(
      "make p1\n"
      "add-task p1 build\n"
      "make p2\n"
      "add-task p2 compile\n"
      "show-project-deps p1\n"),
      "<PROJECT-DEPS: p1, COUNT: 0>\n");
}

BOOST_AUTO_TEST_CASE(invalid_command_scenarios)
{
  const char* invalidCommands[] = {
    "unknown",
    "make",
    "make p1 extra",
    "make 1abc",
    "add-task p2 a",
    "show missing",
    "make p1\nmake p1",
    "make p1\nadd-task p1 1abc",
    "make p1\nadd-task p1 a\nadd-task p1 a",
    "make p1\nadd-task p1 a\nadd-dep p1 a missing",
    "make p1\nadd-task p1 a\nadd-dep p1 a a",
    "make p1\nadd-task p1 a\nremove-dep p1 a b",
    "make p1\nadd-task p1 a\nadd-task p1 b\nadd-dep p1 a b\nadd-dep p1 b a\nbuild-order p1",
    "make p1\nmake p2\nmerge p1 p1 p2",
    "make p1\ncheck-project-dep p1 p2",
    "show-project-deps p2"
  };
  const std::size_t count = sizeof(invalidCommands) / sizeof(invalidCommands[0]);
  for (std::size_t i = 0; i < count; ++i)
  {
    BOOST_CHECK_EQUAL(runCommands(invalidCommands[i]), "<INVALID COMMAND>\n");
  }
}

BOOST_AUTO_TEST_CASE(command_many_projects_and_tasks_regression)
{
  std::string commands;
  for (std::size_t i = 0; i < 35; ++i)
  {
    commands += "make p" + std::to_string(i) + "\n";
  }
  commands += "show p34\n";
  commands += "drop p10\n";
  commands += "make p10\n";
  commands += "show p10\n";
  for (std::size_t i = 0; i < 60; ++i)
  {
    commands += "add-task p0 t" + std::to_string(i) + "\n";
  }
  for (std::size_t i = 1; i < 60; ++i)
  {
    commands += "add-dep p0 t" + std::to_string(i) + " t" + std::to_string(i - 1) + "\n";
  }
  commands += "show p0\n";
  commands += "ready p0\n";
  commands += "build-order p0\n";

  std::string expected;
  expected += "<PROJECT: p34, TASKS: 0, DEPS: 0>\n";
  expected += "<PROJECT: p10, TASKS: 0, DEPS: 0>\n";
  expected += "<PROJECT: p0, TASKS: 60, DEPS: 59>\n";
  expected += "t0\n";
  for (std::size_t i = 0; i < 60; ++i)
  {
    if (i != 0)
    {
      expected += ' ';
    }
    expected += "t" + std::to_string(i);
  }
  expected += '\n';

  BOOST_CHECK_EQUAL(runCommands(commands.c_str()), expected);
}

BOOST_AUTO_TEST_CASE(command_rename_remove_and_reinsert_dependency_regression)
{
  BOOST_CHECK_EQUAL(runCommands(
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
      "show-task p C\n"
      "show-task p D\n"
      "ready p\n"),
      "<TASK: X, DEPENDS-ON: 0, REQUIRED-FOR: 2>\n"
      "<TASK: C, DEPENDS-ON: 1, REQUIRED-FOR: 0>\n"
      "<TASK: D, DEPENDS-ON: 1, REQUIRED-FOR: 0>\n"
      "X\n");
}

BOOST_AUTO_TEST_CASE(command_empty_and_incomplete_lines_regression)
{
  BOOST_CHECK_EQUAL(runCommands(
      "\n"
      "   \n"
      "make\n"
      "show\n"
      "drop\n"
      "add-task p\n"
      "add-dep p a\n"
      "ready\n"),
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n");
}

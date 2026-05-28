#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <string>

#include <sequence.hpp>
#include "task_graph.hpp"

namespace
{
  std::string join(const alekseev::Sequence< std::string >& values)
  {
    std::string result;
    for (std::size_t i = 0; i < values.size(); ++i)
    {
      if (i != 0)
      {
        result += ' ';
      }
      result += values[i];
    }
    return result;
  }
}

BOOST_AUTO_TEST_CASE(task_graph_basic_tasks)
{
  alekseev::TaskGraph graph;

  BOOST_CHECK_EQUAL(graph.taskCount(), 0);
  BOOST_CHECK(graph.addTask("compile"));
  BOOST_CHECK(!graph.addTask("compile"));
  BOOST_CHECK(graph.hasTask("compile"));
  BOOST_CHECK_EQUAL(graph.taskCount(), 1);
  BOOST_CHECK(graph.removeTask("compile"));
  BOOST_CHECK(!graph.hasTask("compile"));
  BOOST_CHECK(!graph.removeTask("compile"));
}

BOOST_AUTO_TEST_CASE(task_graph_rejects_invalid_task_name)
{
  alekseev::TaskGraph graph;

  BOOST_CHECK(!graph.addTask("1compile"));
  BOOST_CHECK(!graph.addTask("compile-task"));
  BOOST_CHECK_EQUAL(graph.taskCount(), 0);
}

BOOST_AUTO_TEST_CASE(task_graph_dependencies)
{
  alekseev::TaskGraph graph;
  BOOST_REQUIRE(graph.addTask("compile"));
  BOOST_REQUIRE(graph.addTask("build"));

  BOOST_CHECK(graph.addDependency("build", "compile"));
  BOOST_CHECK(!graph.addDependency("build", "missing"));
  BOOST_CHECK(!graph.addDependency("build", "build"));
  BOOST_CHECK(!graph.addDependency("build", "compile"));
  BOOST_CHECK(graph.hasDependency("build", "compile"));
  BOOST_CHECK_EQUAL(graph.dependencyCount(), 1);
  BOOST_CHECK_EQUAL(graph.dependsOnCount("build"), 1);
  BOOST_CHECK_EQUAL(graph.requiredForCount("compile"), 1);
  BOOST_CHECK(graph.removeDependency("build", "compile"));
  BOOST_CHECK(!graph.removeDependency("build", "compile"));
  BOOST_CHECK_EQUAL(graph.dependencyCount(), 0);
}

BOOST_AUTO_TEST_CASE(remove_task_cleans_dependencies)
{
  alekseev::TaskGraph graph;
  BOOST_REQUIRE(graph.addTask("compile"));
  BOOST_REQUIRE(graph.addTask("build"));
  BOOST_REQUIRE(graph.addTask("test"));
  BOOST_REQUIRE(graph.addDependency("build", "compile"));
  BOOST_REQUIRE(graph.addDependency("test", "build"));

  BOOST_CHECK(graph.removeTask("build"));

  BOOST_CHECK(!graph.hasTask("build"));
  BOOST_CHECK_EQUAL(graph.dependencyCount(), 0);
  BOOST_CHECK(!graph.hasDependency("build", "compile"));
  BOOST_CHECK(!graph.hasDependency("test", "build"));
  BOOST_CHECK_EQUAL(graph.requiredForCount("compile"), 0);
  BOOST_CHECK_EQUAL(graph.dependsOnCount("test"), 0);
}

BOOST_AUTO_TEST_CASE(rename_task_preserves_dependencies)
{
  alekseev::TaskGraph graph;
  BOOST_REQUIRE(graph.addTask("compile"));
  BOOST_REQUIRE(graph.addTask("build"));
  BOOST_REQUIRE(graph.addTask("test"));
  BOOST_REQUIRE(graph.addDependency("build", "compile"));
  BOOST_REQUIRE(graph.addDependency("test", "build"));

  BOOST_CHECK(graph.renameTask("build", "package"));

  BOOST_CHECK(!graph.hasTask("build"));
  BOOST_CHECK(graph.hasTask("package"));
  BOOST_CHECK(graph.hasDependency("package", "compile"));
  BOOST_CHECK(graph.hasDependency("test", "package"));
  BOOST_CHECK_EQUAL(graph.requiredForCount("compile"), 1);
  BOOST_CHECK_EQUAL(graph.dependencyCount(), 2);
  BOOST_CHECK(!graph.renameTask("package", "compile"));
  BOOST_CHECK(!graph.renameTask("missing", "next"));
}

BOOST_AUTO_TEST_CASE(ready_tasks_keep_insertion_order)
{
  alekseev::TaskGraph graph;
  BOOST_REQUIRE(graph.addTask("compile"));
  BOOST_REQUIRE(graph.addTask("build"));
  BOOST_REQUIRE(graph.addTask("test"));
  BOOST_REQUIRE(graph.addDependency("build", "compile"));
  BOOST_REQUIRE(graph.addDependency("test", "build"));

  const alekseev::Sequence< std::string > ready = graph.getReadyTasks();

  BOOST_REQUIRE_EQUAL(ready.size(), 1);
  BOOST_CHECK_EQUAL(ready[0], "compile");
}

BOOST_AUTO_TEST_CASE(cycle_detection)
{
  alekseev::TaskGraph graph;
  BOOST_REQUIRE(graph.addTask("compile"));
  BOOST_REQUIRE(graph.addTask("build"));
  BOOST_REQUIRE(graph.addDependency("build", "compile"));
  BOOST_CHECK(!graph.hasCycle());

  alekseev::TaskGraph shortCycle;
  BOOST_REQUIRE(shortCycle.addTask("a"));
  BOOST_REQUIRE(shortCycle.addTask("b"));
  BOOST_REQUIRE(shortCycle.addDependency("a", "b"));
  BOOST_REQUIRE(shortCycle.addDependency("b", "a"));
  BOOST_CHECK(shortCycle.hasCycle());

  alekseev::TaskGraph longCycle;
  BOOST_REQUIRE(longCycle.addTask("a"));
  BOOST_REQUIRE(longCycle.addTask("b"));
  BOOST_REQUIRE(longCycle.addTask("c"));
  BOOST_REQUIRE(longCycle.addDependency("a", "b"));
  BOOST_REQUIRE(longCycle.addDependency("b", "c"));
  BOOST_REQUIRE(longCycle.addDependency("c", "a"));
  BOOST_CHECK(longCycle.hasCycle());
}

BOOST_AUTO_TEST_CASE(build_order)
{
  alekseev::TaskGraph graph;
  BOOST_REQUIRE(graph.addTask("compile"));
  BOOST_REQUIRE(graph.addTask("build"));
  BOOST_REQUIRE(graph.addTask("test"));
  BOOST_REQUIRE(graph.addDependency("build", "compile"));
  BOOST_REQUIRE(graph.addDependency("test", "build"));

  alekseev::Sequence< std::string > order;
  BOOST_CHECK(graph.buildOrder(order));
  BOOST_CHECK_EQUAL(join(order), "compile build test");
}

BOOST_AUTO_TEST_CASE(build_order_multiple_ready_tasks)
{
  alekseev::TaskGraph graph;
  BOOST_REQUIRE(graph.addTask("a"));
  BOOST_REQUIRE(graph.addTask("b"));
  BOOST_REQUIRE(graph.addTask("c"));
  BOOST_REQUIRE(graph.addDependency("c", "a"));
  BOOST_REQUIRE(graph.addDependency("c", "b"));

  alekseev::Sequence< std::string > order;
  BOOST_CHECK(graph.buildOrder(order));
  BOOST_CHECK_EQUAL(join(order), "a b c");
}

BOOST_AUTO_TEST_CASE(build_order_reports_cycle)
{
  alekseev::TaskGraph graph;
  BOOST_REQUIRE(graph.addTask("a"));
  BOOST_REQUIRE(graph.addTask("b"));
  BOOST_REQUIRE(graph.addDependency("a", "b"));
  BOOST_REQUIRE(graph.addDependency("b", "a"));

  alekseev::Sequence< std::string > order;
  BOOST_CHECK(!graph.buildOrder(order));
}

BOOST_AUTO_TEST_CASE(task_graph_many_tasks_chain_and_branching)
{
  alekseev::TaskGraph graph;
  const std::size_t taskCount = 75;

  for (std::size_t i = 0; i < taskCount; ++i)
  {
    BOOST_REQUIRE(graph.addTask("t" + std::to_string(i)));
  }
  for (std::size_t i = 1; i < taskCount; ++i)
  {
    BOOST_REQUIRE(graph.addDependency("t" + std::to_string(i), "t" + std::to_string(i - 1)));
  }
  BOOST_REQUIRE(graph.addTask("branchA"));
  BOOST_REQUIRE(graph.addTask("branchB"));
  BOOST_REQUIRE(graph.addTask("final"));
  BOOST_REQUIRE(graph.addDependency("branchA", "t10"));
  BOOST_REQUIRE(graph.addDependency("branchB", "t10"));
  BOOST_REQUIRE(graph.addDependency("final", "branchA"));
  BOOST_REQUIRE(graph.addDependency("final", "branchB"));

  BOOST_CHECK_EQUAL(graph.taskCount(), taskCount + 3);
  BOOST_CHECK_EQUAL(graph.dependsOnCount("final"), 2);
  BOOST_CHECK_EQUAL(graph.requiredForCount("t10"), 3);
  BOOST_CHECK(!graph.hasCycle());

  alekseev::Sequence< std::string > order;
  BOOST_CHECK(graph.buildOrder(order));
  BOOST_CHECK_EQUAL(order.size(), taskCount + 3);
  BOOST_CHECK_EQUAL(order[0], "t0");
  BOOST_CHECK_EQUAL(order[10], "t10");
}

BOOST_AUTO_TEST_CASE(task_graph_remove_dependency_and_tombstone_regression)
{
  alekseev::TaskGraph graph;
  BOOST_REQUIRE(graph.addTask("a"));
  BOOST_REQUIRE(graph.addTask("b"));
  BOOST_REQUIRE(graph.addTask("c"));
  BOOST_REQUIRE(graph.addTask("d"));
  BOOST_REQUIRE(graph.addDependency("b", "a"));
  BOOST_REQUIRE(graph.addDependency("c", "a"));
  BOOST_REQUIRE(graph.addDependency("d", "b"));
  BOOST_REQUIRE(graph.addDependency("d", "c"));

  BOOST_CHECK(graph.removeDependency("d", "b"));
  BOOST_CHECK(!graph.hasDependency("d", "b"));
  BOOST_CHECK_EQUAL(graph.dependsOnCount("d"), 1);
  BOOST_CHECK_EQUAL(graph.requiredForCount("b"), 0);

  BOOST_CHECK(graph.removeTask("a"));
  BOOST_CHECK(!graph.hasTask("a"));
  BOOST_CHECK_EQUAL(graph.dependsOnCount("b"), 0);
  BOOST_CHECK_EQUAL(graph.dependsOnCount("c"), 0);
  BOOST_CHECK_EQUAL(graph.dependencyCount(), 1);
  BOOST_CHECK(graph.addTask("e"));
  BOOST_CHECK(graph.addDependency("e", "d"));
}

BOOST_AUTO_TEST_CASE(task_graph_merge_preserves_unique_tasks_and_edges)
{
  alekseev::TaskGraph left;
  BOOST_REQUIRE(left.addTask("compile"));
  BOOST_REQUIRE(left.addTask("link"));
  BOOST_REQUIRE(left.addDependency("link", "compile"));

  alekseev::TaskGraph right;
  BOOST_REQUIRE(right.addTask("compile"));
  BOOST_REQUIRE(right.addTask("test"));
  BOOST_REQUIRE(right.addDependency("test", "compile"));

  BOOST_CHECK(left.mergeFrom(right));
  BOOST_CHECK_EQUAL(left.taskCount(), 3);
  BOOST_CHECK_EQUAL(left.dependencyCount(), 2);
  BOOST_CHECK(left.hasDependency("link", "compile"));
  BOOST_CHECK(left.hasDependency("test", "compile"));
}

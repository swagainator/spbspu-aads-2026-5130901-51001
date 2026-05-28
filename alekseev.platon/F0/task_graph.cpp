#include "task_graph.hpp"

#include <cstddef>
#include <limits>

#include <queue.hpp>
#include <stack.hpp>

namespace alekseev
{
  namespace
  {
    const std::size_t MissingIndex = std::numeric_limits< std::size_t >::max();
    const int NotVisited = 0;
    const int Visiting = 1;
    const int Visited = 2;
  }

  TaskGraph::TaskGraph():
    tasks_(),
    requires_(101),
    requiredFor_(101),
    dependencyCount_(0)
  {
  }

  bool TaskGraph::hasTask(const std::string& name) const
  {
    for (std::size_t i = 0; i < tasks_.size(); ++i)
    {
      if (tasks_[i] == name)
      {
        return true;
      }
    }
    return false;
  }

  bool TaskGraph::addTask(const std::string& name)
  {
    if (!isValidName(name) || hasTask(name))
    {
      return false;
    }
    tasks_.push_back(name);
    requires_.add(name, StringList());
    requiredFor_.add(name, StringList());
    return true;
  }

  bool TaskGraph::removeTask(const std::string& name)
  {
    const std::size_t index = findTaskIndex(name);
    if (index == MissingIndex)
    {
      return false;
    }

    const StringList depends = requires_.at(name);
    for (std::size_t i = 0; i < depends.size(); ++i)
    {
      if (removeFromList(requiredFor_.at(depends[i]), name))
      {
        --dependencyCount_;
      }
    }

    const StringList dependent = requiredFor_.at(name);
    for (std::size_t i = 0; i < dependent.size(); ++i)
    {
      if (removeFromList(requires_.at(dependent[i]), name))
      {
        --dependencyCount_;
      }
    }

    requires_.drop(name);
    requiredFor_.drop(name);
    tasks_.erase_at(index);
    return true;
  }

  bool TaskGraph::renameTask(const std::string& oldName, const std::string& newName)
  {
    const std::size_t index = findTaskIndex(oldName);
    if (index == MissingIndex || !isValidName(newName) || hasTask(newName))
    {
      return false;
    }

    StringList depends = requires_.drop(oldName);
    StringList dependent = requiredFor_.drop(oldName);
    requires_.add(newName, depends);
    requiredFor_.add(newName, dependent);
    tasks_[index] = newName;

    for (std::size_t i = 0; i < tasks_.size(); ++i)
    {
      replaceInList(requires_.at(tasks_[i]), oldName, newName);
      replaceInList(requiredFor_.at(tasks_[i]), oldName, newName);
    }
    return true;
  }

  bool TaskGraph::hasDependency(const std::string& task, const std::string& dependsOn) const
  {
    if (!hasTask(task) || !hasTask(dependsOn))
    {
      return false;
    }
    return containsInList(requires_.at(task), dependsOn);
  }

  bool TaskGraph::addDependency(const std::string& task, const std::string& dependsOn)
  {
    if (!hasTask(task) || !hasTask(dependsOn) || task == dependsOn ||
        hasDependency(task, dependsOn))
    {
      return false;
    }
    requires_.at(task).push_back(dependsOn);
    requiredFor_.at(dependsOn).push_back(task);
    ++dependencyCount_;
    return true;
  }

  bool TaskGraph::removeDependency(const std::string& task, const std::string& dependsOn)
  {
    if (!hasDependency(task, dependsOn))
    {
      return false;
    }
    removeFromList(requires_.at(task), dependsOn);
    removeFromList(requiredFor_.at(dependsOn), task);
    --dependencyCount_;
    return true;
  }

  std::size_t TaskGraph::taskCount() const noexcept
  {
    return tasks_.size();
  }

  std::size_t TaskGraph::dependencyCount() const noexcept
  {
    return dependencyCount_;
  }

  std::size_t TaskGraph::dependsOnCount(const std::string& task) const
  {
    if (!hasTask(task))
    {
      return 0;
    }
    return requires_.at(task).size();
  }

  std::size_t TaskGraph::requiredForCount(const std::string& task) const
  {
    if (!hasTask(task))
    {
      return 0;
    }
    return requiredFor_.at(task).size();
  }

  Sequence< std::string > TaskGraph::getReadyTasks() const
  {
    Sequence< std::string > result;
    for (std::size_t i = 0; i < tasks_.size(); ++i)
    {
      if (requires_.at(tasks_[i]).empty())
      {
        result.push_back(tasks_[i]);
      }
    }
    return result;
  }

  bool TaskGraph::hasCycle() const
  {
    Sequence< int > colors;
    for (std::size_t i = 0; i < tasks_.size(); ++i)
    {
      colors.push_back(NotVisited);
    }
    for (std::size_t i = 0; i < tasks_.size(); ++i)
    {
      if (colors[i] == NotVisited && hasCycleFrom(tasks_[i], colors))
      {
        return true;
      }
    }
    return false;
  }

  bool TaskGraph::buildOrder(Sequence< std::string >& result) const
  {
    result.clear();
    Sequence< std::size_t > pending;
    Queue< std::string > ready;

    for (std::size_t i = 0; i < tasks_.size(); ++i)
    {
      const std::size_t count = requires_.at(tasks_[i]).size();
      pending.push_back(count);
      if (count == 0)
      {
        ready.push(tasks_[i]);
      }
    }

    while (!ready.empty())
    {
      const std::string current = ready.drop();
      result.push_back(current);
      const StringList& dependent = requiredFor_.at(current);
      for (std::size_t i = 0; i < dependent.size(); ++i)
      {
        const std::size_t index = findTaskIndex(dependent[i]);
        if (index != MissingIndex && pending[index] > 0)
        {
          --pending[index];
          if (pending[index] == 0)
          {
            ready.push(dependent[i]);
          }
        }
      }
    }
    return result.size() == tasks_.size();
  }

  bool TaskGraph::mergeFrom(const TaskGraph& other)
  {
    for (std::size_t i = 0; i < other.tasks_.size(); ++i)
    {
      if (!hasTask(other.tasks_[i]))
      {
        addTask(other.tasks_[i]);
      }
    }
    for (std::size_t i = 0; i < other.tasks_.size(); ++i)
    {
      const StringList& depends = other.requires_.at(other.tasks_[i]);
      for (std::size_t j = 0; j < depends.size(); ++j)
      {
        if (!hasDependency(other.tasks_[i], depends[j]))
        {
          addDependency(other.tasks_[i], depends[j]);
        }
      }
    }
    return true;
  }

  bool TaskGraph::collectDependencies(const std::string& task, StringList& result) const
  {
    result.clear();
    if (!hasTask(task))
    {
      return false;
    }

    StringList visited;
    Stack< std::string > stack;
    const StringList& direct = requires_.at(task);
    for (std::size_t i = 0; i < direct.size(); ++i)
    {
      stack.push(direct[i]);
    }

    while (!stack.empty())
    {
      const std::string current = stack.drop();
      if (containsInList(visited, current))
      {
        continue;
      }
      visited.push_back(current);
      result.push_back(current);

      const StringList& next = requires_.at(current);
      for (std::size_t i = 0; i < next.size(); ++i)
      {
        if (!containsInList(visited, next[i]))
        {
          stack.push(next[i]);
        }
      }
    }
    return true;
  }

  const Sequence< std::string >& TaskGraph::tasks() const noexcept
  {
    return tasks_;
  }

  const Sequence< std::string >& TaskGraph::requiredFor(const std::string& task) const
  {
    return requiredFor_.at(task);
  }

  std::size_t TaskGraph::findTaskIndex(const std::string& name) const
  {
    for (std::size_t i = 0; i < tasks_.size(); ++i)
    {
      if (tasks_[i] == name)
      {
        return i;
      }
    }
    return MissingIndex;
  }

  bool TaskGraph::removeFromList(StringList& values, const std::string& value)
  {
    for (std::size_t i = 0; i < values.size(); ++i)
    {
      if (values[i] == value)
      {
        values.erase_at(i);
        return true;
      }
    }
    return false;
  }

  bool TaskGraph::containsInList(const StringList& values, const std::string& value) const
  {
    for (std::size_t i = 0; i < values.size(); ++i)
    {
      if (values[i] == value)
      {
        return true;
      }
    }
    return false;
  }

  void TaskGraph::replaceInList(StringList& values, const std::string& oldName,
      const std::string& newName)
  {
    for (std::size_t i = 0; i < values.size(); ++i)
    {
      if (values[i] == oldName)
      {
        values[i] = newName;
      }
    }
  }

  bool TaskGraph::hasCycleFrom(const std::string& task, Sequence< int >& colors) const
  {
    const std::size_t index = findTaskIndex(task);
    if (index == MissingIndex)
    {
      return false;
    }
    if (colors[index] == Visiting)
    {
      return true;
    }
    if (colors[index] == Visited)
    {
      return false;
    }

    colors[index] = Visiting;
    const StringList& depends = requires_.at(task);
    for (std::size_t i = 0; i < depends.size(); ++i)
    {
      if (hasCycleFrom(depends[i], colors))
      {
        return true;
      }
    }
    colors[index] = Visited;
    return false;
  }

  bool TaskGraph::StringEqual::operator()(const std::string& lhs, const std::string& rhs) const
  {
    return lhs == rhs;
  }

  std::size_t TaskGraph::HmacHash::operator()(const std::string& value) const
  {
    std::size_t result = 1469598103934665603ull;
    for (std::size_t i = 0; i < value.size(); ++i)
    {
      result ^= static_cast< unsigned char >(value[i]);
      result *= 1099511628211ull;
    }
    return result;
  }
}

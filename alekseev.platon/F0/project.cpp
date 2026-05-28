#include "project.hpp"

namespace alekseev
{
  Project::Project():
    name_(),
    graph_()
  {
  }

  Project::Project(const std::string& name):
    name_(name),
    graph_()
  {
  }

  const std::string& Project::name() const noexcept
  {
    return name_;
  }

  TaskGraph& Project::graph() noexcept
  {
    return graph_;
  }

  const TaskGraph& Project::graph() const noexcept
  {
    return graph_;
  }
}

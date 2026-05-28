#ifndef ALEKSEEV_F0_PROJECT_HPP
#define ALEKSEEV_F0_PROJECT_HPP

#include <string>

#include "task_graph.hpp"

namespace alekseev
{
  class Project
  {
  public:
    Project();
    explicit Project(const std::string& name);

    const std::string& name() const noexcept;
    TaskGraph& graph() noexcept;
    const TaskGraph& graph() const noexcept;

  private:
    std::string name_;
    TaskGraph graph_;
  };
}

#endif

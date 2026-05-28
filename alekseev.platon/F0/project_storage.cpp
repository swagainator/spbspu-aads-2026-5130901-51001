#include "project_storage.hpp"

#include <cstddef>
#include <ostream>

#include <sequence.hpp>
#include "name_utils.hpp"

namespace alekseev
{
  bool ProjectStorage::makeProject(const std::string& name)
  {
    if (!isValidName(name) || hasProject(name))
    {
      return false;
    }
    projects_.push(name, Project(name));
    return true;
  }

  bool ProjectStorage::dropProject(const std::string& name)
  {
    if (!hasProject(name))
    {
      return false;
    }
    projects_.drop(name);
    return true;
  }

  bool ProjectStorage::hasProject(const std::string& name) const
  {
    return projects_.contains(name);
  }

  Project& ProjectStorage::getProject(const std::string& name)
  {
    return projects_.get(name);
  }

  const Project& ProjectStorage::getProject(const std::string& name) const
  {
    return projects_.get(name);
  }

  bool ProjectStorage::showProject(const std::string& name, std::ostream& out) const
  {
    if (!hasProject(name))
    {
      return false;
    }
    const Project& project = projects_.get(name);
    out << "<PROJECT: " << name << ", TASKS: " << project.graph().taskCount()
        << ", DEPS: " << project.graph().dependencyCount() << ">\n";
    return true;
  }

  bool ProjectStorage::mergeProjects(const std::string& newName, const std::string& leftName,
      const std::string& rightName)
  {
    if (!isValidName(newName) || hasProject(newName) || !hasProject(leftName) ||
        !hasProject(rightName))
    {
      return false;
    }

    Project merged(newName);
    merged.graph().mergeFrom(projects_.get(leftName).graph());
    merged.graph().mergeFrom(projects_.get(rightName).graph());
    projects_.push(newName, merged);
    return true;
  }

  bool ProjectStorage::checkProjectDependency(const std::string& leftName,
      const std::string& rightName) const
  {
    if (!hasProject(leftName) || !hasProject(rightName))
    {
      return false;
    }
    return projectDependsOn(projects_.get(leftName), projects_.get(rightName));
  }

  bool ProjectStorage::showProjectDependencies(const std::string& name, std::ostream& out) const
  {
    if (!hasProject(name))
    {
      return false;
    }

    Sequence< std::string > names;
    const Project& project = projects_.get(name);
    for (RBTree< std::string, Project >::const_iterator it = projects_.cbegin();
        it != projects_.cend(); ++it)
    {
      if (it->first != name && projectDependsOn(project, it->second))
      {
        names.push_back(it->first);
      }
    }

    out << "<PROJECT-DEPS: " << name << ", COUNT: " << names.size() << ">\n";
    for (std::size_t i = 0; i < names.size(); ++i)
    {
      out << names[i] << '\n';
    }
    return true;
  }

  bool ProjectStorage::projectDependsOn(const Project& left, const Project& right) const
  {
    const Sequence< std::string >& leftTasks = left.graph().tasks();
    for (std::size_t i = 0; i < leftTasks.size(); ++i)
    {
      Sequence< std::string > deps;
      left.graph().collectDependencies(leftTasks[i], deps);
      for (std::size_t j = 0; j < deps.size(); ++j)
      {
        if (right.graph().hasTask(deps[j]))
        {
          return true;
        }
      }
    }
    return false;
  }
}

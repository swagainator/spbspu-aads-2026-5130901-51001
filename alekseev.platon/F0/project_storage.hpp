#ifndef ALEKSEEV_F0_PROJECT_STORAGE_HPP
#define ALEKSEEV_F0_PROJECT_STORAGE_HPP

#include <iosfwd>
#include <string>

#include "project.hpp"
#include "rb_tree.hpp"

namespace alekseev
{
  class ProjectStorage
  {
  public:
    bool makeProject(const std::string& name);
    bool dropProject(const std::string& name);
    bool hasProject(const std::string& name) const;
    Project& getProject(const std::string& name);
    const Project& getProject(const std::string& name) const;
    bool showProject(const std::string& name, std::ostream& out) const;
    bool mergeProjects(const std::string& newName, const std::string& leftName,
        const std::string& rightName);
    bool checkProjectDependency(const std::string& leftName,
        const std::string& rightName) const;
    bool showProjectDependencies(const std::string& name, std::ostream& out) const;

  private:
    RBTree< std::string, Project > projects_;

    bool projectDependsOn(const Project& left, const Project& right) const;
  };
}

#endif

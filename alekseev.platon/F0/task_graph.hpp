#ifndef ALEKSEEV_F0_TASK_GRAPH_HPP
#define ALEKSEEV_F0_TASK_GRAPH_HPP

#include <cstddef>
#include <string>

#include <sequence.hpp>
#include "double_hash_table.hpp"
#include "name_utils.hpp"

namespace alekseev
{
  class TaskGraph
  {
  public:
    TaskGraph();

    bool hasTask(const std::string& name) const;
    bool addTask(const std::string& name);
    bool removeTask(const std::string& name);
    bool renameTask(const std::string& oldName, const std::string& newName);
    bool hasDependency(const std::string& task, const std::string& dependsOn) const;
    bool addDependency(const std::string& task, const std::string& dependsOn);
    bool removeDependency(const std::string& task, const std::string& dependsOn);
    std::size_t taskCount() const noexcept;
    std::size_t dependencyCount() const noexcept;
    std::size_t dependsOnCount(const std::string& task) const;
    std::size_t requiredForCount(const std::string& task) const;
    Sequence< std::string > getReadyTasks() const;
    bool hasCycle() const;
    bool buildOrder(Sequence< std::string >& result) const;
    bool mergeFrom(const TaskGraph& other);
    bool collectDependencies(const std::string& task, Sequence< std::string >& result) const;
    const Sequence< std::string >& tasks() const noexcept;
    const Sequence< std::string >& requiredFor(const std::string& task) const;

  private:
    struct StringEqual
    {
      bool operator()(const std::string& lhs, const std::string& rhs) const;
    };

    struct HmacHash
    {
      std::size_t operator()(const std::string& value) const;
    };

    using StringList = Sequence< std::string >;
    using DependencyTable =
        DoubleHashTable< std::string, StringList, HmacHash, StringEqual >;

    StringList tasks_;
    DependencyTable requires_;
    DependencyTable requiredFor_;
    std::size_t dependencyCount_;

    std::size_t findTaskIndex(const std::string& name) const;
    bool removeFromList(StringList& values, const std::string& value);
    bool containsInList(const StringList& values, const std::string& value) const;
    void replaceInList(StringList& values, const std::string& oldName,
        const std::string& newName);
    bool hasCycleFrom(const std::string& task, Sequence< int >& colors) const;
  };
}

#endif

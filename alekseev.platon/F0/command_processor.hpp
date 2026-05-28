#ifndef ALEKSEEV_F0_COMMAND_PROCESSOR_HPP
#define ALEKSEEV_F0_COMMAND_PROCESSOR_HPP

#include <iosfwd>
#include <string>

#include <sequence.hpp>
#include "project_storage.hpp"

namespace alekseev
{
  const char F0InvalidCommand[] = "<INVALID COMMAND>";

  class CommandProcessor
  {
  public:
    CommandProcessor(ProjectStorage& storage, std::ostream& out);

    void processLine(const std::string& line);

  private:
    using Args = Sequence< std::string >;

    void printInvalid();
    Args parseArgs(const std::string& line) const;
    void handleMake(const Args& args);
    void handleShow(const Args& args);
    void handleDrop(const Args& args);
    void handleAddTask(const Args& args);
    void handleRemoveTask(const Args& args);
    void handleRenameTask(const Args& args);
    void handleAddDep(const Args& args);
    void handleRemoveDep(const Args& args);
    void handleShowTask(const Args& args);
    void handleCheckCycle(const Args& args);
    void handleBuildOrder(const Args& args);
    void handleReady(const Args& args);
    void handleMerge(const Args& args);
    void handleCheckProjectDep(const Args& args);
    void handleShowProjectDeps(const Args& args);

    ProjectStorage& storage_;
    std::ostream& out_;
  };
}

#endif

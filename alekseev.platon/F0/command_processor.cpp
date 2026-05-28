#include "command_processor.hpp"

#include <cstddef>
#include <ostream>
#include <stdexcept>

#include <line_parser.hpp>

namespace alekseev
{
  namespace
  {
    const std::size_t CommandIndex = 0;
    const std::size_t ProjectIndex = 1;
    const std::size_t TaskIndex = 2;
    const std::size_t DependsOnIndex = 3;
    const std::size_t OldTaskIndex = 2;
    const std::size_t NewTaskIndex = 3;
    const std::size_t NewProjectIndex = 1;
    const std::size_t LeftProjectIndex = 2;
    const std::size_t RightProjectIndex = 3;
    const std::size_t OneArgCommandSize = 2;
    const std::size_t TwoArgCommandSize = 3;
    const std::size_t ThreeArgCommandSize = 4;
  }

  CommandProcessor::CommandProcessor(ProjectStorage& storage, std::ostream& out):
    storage_(storage),
    out_(out)
  {
  }

  void CommandProcessor::processLine(const std::string& line)
  {
    try
    {
      const Args args = parseArgs(line);
      if (args.empty())
      {
        return;
      }

      const std::string& command = args[CommandIndex];
      if (command == "make")
      {
        handleMake(args);
      }
      else if (command == "show")
      {
        handleShow(args);
      }
      else if (command == "drop")
      {
        handleDrop(args);
      }
      else if (command == "add-task")
      {
        handleAddTask(args);
      }
      else if (command == "remove-task")
      {
        handleRemoveTask(args);
      }
      else if (command == "rename-task")
      {
        handleRenameTask(args);
      }
      else if (command == "add-dep")
      {
        handleAddDep(args);
      }
      else if (command == "remove-dep")
      {
        handleRemoveDep(args);
      }
      else if (command == "show-task")
      {
        handleShowTask(args);
      }
      else if (command == "check-cycle")
      {
        handleCheckCycle(args);
      }
      else if (command == "build-order")
      {
        handleBuildOrder(args);
      }
      else if (command == "ready")
      {
        handleReady(args);
      }
      else if (command == "merge")
      {
        handleMerge(args);
      }
      else if (command == "check-project-dep")
      {
        handleCheckProjectDep(args);
      }
      else if (command == "show-project-deps")
      {
        handleShowProjectDeps(args);
      }
      else
      {
        printInvalid();
      }
    }
    catch (const std::exception&)
    {
      printInvalid();
    }
  }

  void CommandProcessor::printInvalid()
  {
    out_ << F0InvalidCommand << '\n';
  }

  CommandProcessor::Args CommandProcessor::parseArgs(const std::string& line) const
  {
    Args args;
    LineParser parser(line);
    while (parser.hasNext())
    {
      args.push_back(parser.readWord());
    }
    return args;
  }

  void CommandProcessor::handleMake(const Args& args)
  {
    if (args.size() != OneArgCommandSize || !storage_.makeProject(args[ProjectIndex]))
    {
      printInvalid();
    }
  }

  void CommandProcessor::handleShow(const Args& args)
  {
    if (args.size() != OneArgCommandSize || !storage_.showProject(args[ProjectIndex], out_))
    {
      printInvalid();
    }
  }

  void CommandProcessor::handleDrop(const Args& args)
  {
    if (args.size() != OneArgCommandSize || !storage_.dropProject(args[ProjectIndex]))
    {
      printInvalid();
    }
  }

  void CommandProcessor::handleAddTask(const Args& args)
  {
    if (args.size() != TwoArgCommandSize || !storage_.hasProject(args[ProjectIndex]) ||
        !storage_.getProject(args[ProjectIndex]).graph().addTask(args[TaskIndex]))
    {
      printInvalid();
    }
  }

  void CommandProcessor::handleRemoveTask(const Args& args)
  {
    if (args.size() != TwoArgCommandSize || !storage_.hasProject(args[ProjectIndex]) ||
        !storage_.getProject(args[ProjectIndex]).graph().removeTask(args[TaskIndex]))
    {
      printInvalid();
    }
  }

  void CommandProcessor::handleRenameTask(const Args& args)
  {
    if (args.size() != ThreeArgCommandSize || !storage_.hasProject(args[ProjectIndex]) ||
        !storage_.getProject(args[ProjectIndex]).graph().renameTask(
            args[OldTaskIndex], args[NewTaskIndex]))
    {
      printInvalid();
    }
  }

  void CommandProcessor::handleAddDep(const Args& args)
  {
    if (args.size() != ThreeArgCommandSize || !storage_.hasProject(args[ProjectIndex]) ||
        !storage_.getProject(args[ProjectIndex]).graph().addDependency(
            args[TaskIndex], args[DependsOnIndex]))
    {
      printInvalid();
    }
  }

  void CommandProcessor::handleRemoveDep(const Args& args)
  {
    if (args.size() != ThreeArgCommandSize || !storage_.hasProject(args[ProjectIndex]) ||
        !storage_.getProject(args[ProjectIndex]).graph().removeDependency(
            args[TaskIndex], args[DependsOnIndex]))
    {
      printInvalid();
    }
  }

  void CommandProcessor::handleShowTask(const Args& args)
  {
    if (args.size() != TwoArgCommandSize || !storage_.hasProject(args[ProjectIndex]))
    {
      printInvalid();
      return;
    }

    const TaskGraph& graph = storage_.getProject(args[ProjectIndex]).graph();
    if (!graph.hasTask(args[TaskIndex]))
    {
      printInvalid();
      return;
    }
    out_ << "<TASK: " << args[TaskIndex] << ", DEPENDS-ON: "
        << graph.dependsOnCount(args[TaskIndex]) << ", REQUIRED-FOR: "
        << graph.requiredForCount(args[TaskIndex]) << ">\n";
  }

  void CommandProcessor::handleCheckCycle(const Args& args)
  {
    if (args.size() != OneArgCommandSize || !storage_.hasProject(args[ProjectIndex]))
    {
      printInvalid();
      return;
    }
    out_ << "<CYCLE: "
        << (storage_.getProject(args[ProjectIndex]).graph().hasCycle() ? "YES" : "NO")
        << ">\n";
  }

  void CommandProcessor::handleBuildOrder(const Args& args)
  {
    if (args.size() != OneArgCommandSize || !storage_.hasProject(args[ProjectIndex]))
    {
      printInvalid();
      return;
    }

    Sequence< std::string > order;
    if (!storage_.getProject(args[ProjectIndex]).graph().buildOrder(order))
    {
      printInvalid();
      return;
    }
    for (std::size_t i = 0; i < order.size(); ++i)
    {
      if (i != 0)
      {
        out_ << ' ';
      }
      out_ << order[i];
    }
    out_ << '\n';
  }

  void CommandProcessor::handleReady(const Args& args)
  {
    if (args.size() != OneArgCommandSize || !storage_.hasProject(args[ProjectIndex]))
    {
      printInvalid();
      return;
    }
    const Sequence< std::string > ready =
        storage_.getProject(args[ProjectIndex]).graph().getReadyTasks();
    if (ready.empty())
    {
      out_ << '\n';
      return;
    }
    for (std::size_t i = 0; i < ready.size(); ++i)
    {
      out_ << ready[i] << '\n';
    }
  }

  void CommandProcessor::handleMerge(const Args& args)
  {
    if (args.size() != ThreeArgCommandSize || !storage_.mergeProjects(
        args[NewProjectIndex], args[LeftProjectIndex], args[RightProjectIndex]))
    {
      printInvalid();
    }
  }

  void CommandProcessor::handleCheckProjectDep(const Args& args)
  {
    if (args.size() != TwoArgCommandSize || !storage_.hasProject(args[ProjectIndex]) ||
        !storage_.hasProject(args[TaskIndex]))
    {
      printInvalid();
      return;
    }
    out_ << "<PROJECT-DEP: "
        << (storage_.checkProjectDependency(args[ProjectIndex], args[TaskIndex]) ? "YES" : "NO")
        << ">\n";
  }

  void CommandProcessor::handleShowProjectDeps(const Args& args)
  {
    if (args.size() != OneArgCommandSize ||
        !storage_.showProjectDependencies(args[ProjectIndex], out_))
    {
      printInvalid();
    }
  }
}

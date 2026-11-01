/**
 * @file WorkflowEngine.h
 * @brief Facade over the TIVIDY subsystems (Facade pattern).
 *
 * One call here hides a sequence across several subsystems:
 *   assembleBlueprint -> Director, Builder
 *   instantiateInstance     -> Prototype, Mediator, Strategy
 *   startInstance     -> Mediator, Command, State
 *   complete/reject/cancelTask -> Mediator, Command, State
 *   setStrategy       -> Strategy
 *   setAdapter/importData -> Adapter, Command
 *   undo/redo         -> Command
 *   getTaskRows       -> Composite, Iterator, State
 */

#ifndef WORKFLOWENGINE_H
#define WORKFLOWENGINE_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "CustomDefinitionBuilder.h"
#include "Director.h"
#include "WorkflowDefinition.h"
#include "WorkflowInstance.h"

class EngineMediator;
class AssignmentStrategy;
class DataAdapter;
class WorkflowComponent;

// one line of the task tree, so the tui never touches a task pointer
struct TaskRow
{
    int depth;
    std::string name;
    std::string state;
    std::string assignee; //"" if nobody
    std::string outcome;
    std::string condition;
    bool isGroup; // true for a subprocess
    bool skipped; // true if the task's branch condition does not hold
};

class WorkflowEngine
{
private:
    // members are destroyed in reverse order, so mediators go before instances,
    // and instances before the WorkflowDefinitions their participants belong to
    CustomDefinitionBuilder builder;
    Director director;
    std::map<std::string, std::unique_ptr<WorkflowDefinition>> blueprints;
    std::map<int, std::unique_ptr<WorkflowInstance>> instances;
    std::map<int, std::unique_ptr<EngineMediator>> mediators;
    std::map<int, std::string> adapterNames;
    std::vector<std::string> activityLog;
    std::string lastError;
    int nextGlobalInstanceId;

    void logEvent(const std::string &message);
    bool fail(const std::string &message);
    WorkflowInstance *findInstance(int id) const;
    EngineMediator *findMediator(int id) const;

    static AssignmentStrategy *makeStrategy(const std::string &name);
    static DataAdapter *makeAdapter(const std::string &name);
    static bool stateIs(WorkflowComponent *c, const char *name);
    static void collectRows(WorkflowComponent *node, int depth, std::vector<TaskRow> &out);
    static WorkflowComponent *findNode(WorkflowInstance *inst, int index);

public:
    WorkflowEngine();
    ~WorkflowEngine();
    WorkflowEngine(const WorkflowEngine &) = delete;
    WorkflowEngine &operator=(const WorkflowEngine &) = delete;

    // builds a WorkflowDefinition with the director and stores it. false if anything is wrong, see getLastError()
    bool assembleBlueprint(const std::string &name,
                           int version,
                           const std::vector<std::string> &departments,
                           const std::vector<EmployeeSpec> &employees,
                           const std::vector<SubprocessSpec> &subprocesses,
                           const std::vector<TaskSpec> &tasks);
    std::vector<std::string> getBlueprintNames() const;

    // clones a WorkflowDefinition into a new instance with its own mediator. returns the id, or -1
    int instantiateInstance(const std::string &blueprintName,
                            const std::string &strategyName = "LeastLoaded");
    std::vector<int> getActiveInstanceIds() const;
    std::string getInstanceLabel(int instanceId) const;
    std::string getInstanceStatus(int instanceId) const; // Not started, Running, Halted, Finished

    bool startInstance(int instanceId);

    // taskIndex is the position in getTaskRows()
    bool completeTask(int instanceId, int taskIndex);
    bool rejectTask(int instanceId, int taskIndex);
    bool cancelTask(int instanceId, int taskIndex);
    std::vector<TaskRow> getTaskRows(int instanceId) const;
    // names of the tasks that are Complete (uses the CompletedTaskIterator)
    std::vector<std::string> getCompletedTasks(int instanceId) const;

    std::vector<std::string> getStrategyNames() const;
    bool setStrategy(int instanceId, const std::string &strategyName);
    std::string getStrategyName(int instanceId) const;

    std::vector<std::string> getAdapterNames() const;
    bool setAdapter(int instanceId, const std::string &adapterName);
    std::string getAdapterName(int instanceId) const;
    bool importData(int instanceId, const std::string &recordId); // one macro command, one undo

    bool setPayloadValue(int instanceId, const std::string &key, const std::string &value);
    std::map<std::string, std::string> getPayloadMap(int instanceId) const;

    bool undo(int instanceId);
    bool redo(int instanceId);
    std::vector<std::string> getHistory(int instanceId) const;

    std::vector<std::string> generateReport(int instanceId) const;
    bool exportReport(const std::string &path);

    const std::vector<std::string> &getActivityLog() const;
    const std::string &getLastError() const;
};

#endif
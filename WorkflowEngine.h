/**
 * @file WorkflowEngine.h
 * @brief Facade over the TIVIDY subsystems.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - GoF Pattern: Facade
 * - Role Explanation:
 *   One call here hides a sequence across several subsystems, so the TUI does
 *   not need to know the order or the wiring:
 *     assembleBlueprint -> Director, Builder, blueprint registry
 *     spawnInstance     -> Prototype (clone), Mediator, Strategy, Composite
 *     startInstance     -> Mediator, Command history, State, Strategy
 *     completeTask etc. -> Mediator, Command history, State
 *     setStrategy       -> Strategy (swapped at runtime)
 *     setAdapter/import -> Adapter, Command (one undoable macro), DataPayload
 *     undo / redo       -> Command history
 *     getTaskRows       -> Composite, Iterator, State
 *     generateReport    -> all of the above
 *   The TUI may still use other subsystems directly; the facade owns the
 *   lifetime of blueprints, instances and mediators.
 * ============================================================================
 */

#ifndef WORKFLOWENGINE_H
#define WORKFLOWENGINE_H

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "CustomDefinitionBuilder.h"
#include "Director.h"
#include "WorkflowDefinition.h"
#include "WorkflowInstance.h"

class EngineMediator;
class OutputCapture; // private helper, defined in WorkflowEngine.cpp

/**
 * @struct TaskRow
 * @brief One line of an instance's task tree, in a form the TUI can draw.
 *
 * Rows are numbered in tree order (parent before children, left to right).
 * The same number is passed back to completeTask(), rejectTask() and
 * cancelTask(), so the TUI never holds a Task pointer.
 */
struct TaskRow
{
    int index;             ///< Position in tree order, starting at 0 (the root).
    int depth;             ///< Nesting level, 0 for the root.
    std::string name;      ///< Task or subprocess name.
    std::string state;     ///< NotStarted, InProgress, Complete, Rejected or Cancelled.
    std::string assignee;  ///< Who the task is assigned to, or "" if nobody.
    std::string outcome;   ///< Recorded decision, or "".
    std::string condition; ///< Branch condition (key=value), or "".
    bool isGroup;          ///< True for a subprocess, false for a single task.
};

/**
 * @class WorkflowEngine
 * @brief The Facade. Not copyable.
 */
class WorkflowEngine
{
private:
    friend class OutputCapture;

    enum class TaskAction
    {
        Complete,
        Reject,
        Cancel
    };

    // Declaration order matters: members are destroyed in reverse, so the
    // mediators go first, then the instances, then the blueprints whose
    // participants the instances still point at.
    CustomDefinitionBuilder builder;
    Director director;
    std::map<std::string, std::unique_ptr<WorkflowDefinition>> blueprints;
    std::map<int, std::unique_ptr<WorkflowInstance>> instances;
    std::map<int, std::unique_ptr<EngineMediator>> mediators;
    std::map<int, std::string> adapterNames; ///< Adapter chosen per instance.
    std::vector<std::string> activityLog;
    std::string lastError;
    std::ostringstream captured; ///< Subsystem output waiting to be moved into the log.
    int nextGlobalInstanceId;

    bool registerBlueprint();
    void logEvent(const std::string &message);
    bool fail(const std::string &message);
    void drainCaptured();
    WorkflowInstance *findInstance(int id) const;
    EngineMediator *findMediator(int id) const;
    bool runTaskAction(int instanceId, int taskIndex, TaskAction action);

public:
    WorkflowEngine();
    ~WorkflowEngine();
    WorkflowEngine(const WorkflowEngine &) = delete;
    WorkflowEngine &operator=(const WorkflowEngine &) = delete;

    // ---------------- Blueprints (Director + Builder) ----------------

    /**
     * @brief Builds a blueprint through the Director and registers it.
     *
     * Fails (and returns false, see getLastError()) if the name already exists,
     * there are no tasks, a department has no employees, or the builder
     * rejects the data (unknown parent, duplicate name, ...).
     */
    bool assembleBlueprint(const std::string &name,
                           int version,
                           const std::vector<std::string> &departments,
                           const std::vector<EmployeeSpec> &employees,
                           const std::vector<SubprocessSpec> &subprocesses,
                           const std::vector<TaskSpec> &tasks);

    /** @brief Names of all registered blueprints. */
    std::vector<std::string> getBlueprintNames() const;

    // ---------------- Instances (Prototype + Mediator + Strategy) ----------------

    /**
     * @brief Clones a blueprint into a new instance with its own mediator.
     * @param blueprintName Registered blueprint.
     * @param strategyName "LeastLoaded", "RoleBased" or "MostExperience".
     * @return The new instance ID (> 0), or -1 on failure.
     */
    int spawnInstance(const std::string &blueprintName,
                      const std::string &strategyName = "LeastLoaded");

    /** @brief Destroys an instance and its mediator. */
    bool removeInstance(int instanceId);

    /** @brief IDs of all instances, oldest first. */
    std::vector<int> getActiveInstanceIds() const;

    /** @brief Menu label such as "#2  Retail_Return_Item v1  [Running]". */
    std::string getInstanceLabel(int instanceId) const;

    /** @brief "Not started", "Running", "Halted", "Finished" or "Unknown". */
    std::string getInstanceStatus(int instanceId) const;

    // ---------------- Execution (Mediator + Command + State) ----------------

    /** @brief Starts the workflow; the mediator starts whatever is ready first. */
    bool startInstance(int instanceId);

    /** @brief Completes the task at the given row index (see TaskRow). */
    bool completeTask(int instanceId, int taskIndex);

    /** @brief Rejects the task at the given row index. */
    bool rejectTask(int instanceId, int taskIndex);

    /** @brief Cancels the task at the given row index. */
    bool cancelTask(int instanceId, int taskIndex);

    /** @brief Completes the first single task that is currently InProgress. */
    bool completeNextTask(int instanceId);

    /** @brief The instance's task tree as rows (Composite + Iterator + State). */
    std::vector<TaskRow> getTaskRows(int instanceId) const;

    // ---------------- Strategy ----------------

    /** @brief Names accepted by spawnInstance() and setStrategy(). */
    std::vector<std::string> getStrategyNames() const;

    /** @brief Swaps the assignment strategy of a running instance. */
    bool setStrategy(int instanceId, const std::string &strategyName);

    /** @brief Name of the strategy in use, or "(none)". */
    std::string getStrategyName(int instanceId) const;

    // ---------------- Adapter ----------------

    /** @brief Names accepted by setAdapter(): "claims" and "hr". */
    std::vector<std::string> getAdapterNames() const;

    /** @brief Connects an external-data adapter to an instance. */
    bool setAdapter(int instanceId, const std::string &adapterName);

    /** @brief Name of the adapter in use, or "none". */
    std::string getAdapterName(int instanceId) const;

    /**
     * @brief Loads a record through the adapter into the instance's payload.
     * It is one macro command, so it is one undo step.
     */
    bool importData(int instanceId, const std::string &recordId);

    // ---------------- Payload and history (Command) ----------------

    /** @brief Sets a payload value as an undoable command. */
    bool setPayloadValue(int instanceId, const std::string &key, const std::string &value);

    /** @brief Copy of the instance's payload. */
    std::map<std::string, std::string> getPayloadMap(int instanceId) const;

    /** @brief Undoes the instance's most recent action. */
    bool undo(int instanceId);

    /** @brief Redoes the most recently undone action. */
    bool redo(int instanceId);

    /** @brief One line per command, oldest first; undone ones are marked. */
    std::vector<std::string> getHistory(int instanceId) const;

    // ---------------- Reports and log ----------------

    /** @brief Full report for one instance. */
    std::vector<std::string> generateReport(int instanceId) const;

    /** @brief Summary of blueprints, instances, strategies and adapters. */
    std::vector<std::string> generateSystemReport() const;

    /** @brief Writes the system report, every instance report and the log to a text file. */
    bool exportReport(const std::string &path);

    /** @brief Engine and subsystem events, oldest first (capped at 500 lines). */
    const std::vector<std::string> &getActivityLog() const;

    /** @brief Reason the last failed call failed ("" if it succeeded). */
    const std::string &getLastError() const;
};

#endif // WORKFLOWENGINE_H
/**
 * @file WorkflowEngine.h
 * @brief Facade Pattern providing a unified interface to the TIVIDY Workflow subsystem.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - GoF Pattern: Facade Pattern (Participant: Facade)
 * - Role Explanation:
 *   Provides a clean, high-level API for the TUI (FTXUI) and client code without
 *   exposing internal pointers to Composite nodes, Builders, Prototypes, or
 *   Mediators. Orchestrates blueprint assembly (Builder), instance creation
 *   (Prototype), runtime execution (EngineMediator), and live TUI state queries.
 * ============================================================================
 */

#ifndef WORKFLOWENGINE_H
#define WORKFLOWENGINE_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "CustomDefinitionBuilder.h"
#include "Director.h"
#include "WorkflowDefinition.h"
#include "WorkflowInstance.h"
// #include "EngineMediator.h"

/**
 * @struct EmployeeSpec
 * @brief Input structure for passing an employee into assembleBlueprint().
 */
struct EmployeeSpec
{
    std::string name;
    std::string department; // "" if standalone
};

/**
 * @struct SubprocessSpec
 * @brief Input structure for passing a subprocess into assembleBlueprint().
 */
struct SubprocessSpec
{
    std::string name;
    std::string parent; // "ROOT" or parent subprocess name
};

/**
 * @struct TaskSpec
 * @brief Input structure for passing a task into assembleBlueprint().
 */
struct TaskSpec
{
    std::string name;
    std::string parent; // "ROOT" or parent subprocess name
    int priority;
    std::string branchCondition;
};

class WorkflowEngine
{
private:
    /** @brief Concrete Builder used to assemble workflow definitions. */
    CustomDefinitionBuilder builder;

    /** @brief Optional GoF Director for standard construction sequences. */
    Director director;

    /** @brief Registry of completed WorkflowDefinition blueprints by name. */
    std::map<std::string, std::unique_ptr<WorkflowDefinition>> blueprints;

    /** @brief Registry of active WorkflowInstances by instance ID. */
    std::map<int, std::unique_ptr<WorkflowInstance>> instances;

    /** @brief Registry of EngineMediators managing each active instance ID. */
    // std::map<int, std::unique_ptr<EngineMediator>> mediators;

    /** @brief Running log of engine events for the FTXUI console panel. */
    std::vector<std::string> activityLog;

    /** @brief Counter to ensure globally unique instance IDs across the engine. */
    int nextGlobalInstanceId;

    /** @brief Helper to append timestamped/formatted entries to the TUI log. */
    void logEvent(const std::string &message);

public:
    WorkflowEngine();
    ~WorkflowEngine();

    // ========================================================================
    // GROUP A: Blueprint Assembly (Builder Pattern)
    // ========================================================================

    /**
     * @brief Assembles a complete WorkflowDefinition from structured input in one go,
     *        or leaves it open if finalizeNow is false so more items can be added.
     * @param name Blueprint name (e.g., "Retail_Return_Item", "Bank_Loan").
     * @param version Blueprint version number.
     * @param departments List of department names to create.
     * @param employees List of employees (and their target departments).
     * @param subprocesses List of composite subprocesses (and their parent names).
     * @param tasks List of leaf tasks (with parent, priority, and condition).
     * @param finalizeNow If true, calls finalizeCustomBlueprint() immediately.
     */
    void assembleBlueprint(const std::string &name,
                           int version,
                           const std::vector<std::string> &departments,
                           const std::vector<EmployeeSpec> &employees,
                           const std::vector<SubprocessSpec> &subprocesses,
                           const std::vector<TaskSpec> &tasks,
                           bool finalizeNow = true);

    /**
     * @brief Starts a fresh custom blueprint with a specific name and version.
     */
    void startCustomBlueprint(const std::string &name, int version = 1);

    /**
     * @brief Adds a Department composite participant to the active build.
     */
    void addCustomDepartment(const std::string &deptName);

    /**
     * @brief Wrapper around builder.addEmployee(...) to add an employee during/after input.
     */
    void addCustomEmployee(const std::string &empName, const std::string &deptName = "");

    /**
     * @brief Wrapper around builder.addSubprocess(...) to add a subprocess container.
     */
    void addCustomSubprocess(const std::string &subName, const std::string &parent = "ROOT");

    /**
     * @brief Wrapper around builder.addTask(...) to add a leaf task.
     */
    void addCustomTask(const std::string &taskName,
                       const std::string &parent = "ROOT",
                       int priority = 1,
                       const std::string &condition = "");

    /**
     * @brief Extracts the finished WorkflowDefinition from the builder and stores it.
     * @return True if a definition was successfully finalized and stored.
     */
    bool finalizeCustomBlueprint();

    // ========================================================================
    // Instance Spawning & Lifecycle (Prototype & Mediator Patterns)
    // ========================================================================

    /**
     * @brief Spawns an isolated WorkflowInstance from a stored blueprint using Prototype::clone(),
     *        attaches a dedicated EngineMediator, and wires the participant candidates.
     * @param blueprintName Name of the registered WorkflowDefinition.
     * @return Unique instance ID (> 0), or -1 if the blueprint was not found.
     */
    int spawnInstance(const std::string &blueprintName);

    /**
     * @brief Starts execution of a spawned WorkflowInstance via its EngineMediator.
     */
    // void startInstance(int instanceId);

    /**
     * @brief Stops/cancels an active WorkflowInstance.
     */
    // void cancelInstance(int instanceId);

    // ========================================================================
    // TUI Data & View Getters (Zero Raw Pointers)
    // ========================================================================

    /**
     * @brief Returns the names of all finalized blueprints (for FTXUI Menus).
     */
    std::vector<std::string> getBlueprintNames() const;

    /**
     * @brief Returns the IDs of all spawned instances (for FTXUI Menus).
     */
    std::vector<int> getActiveInstanceIds() const;

    /**
     * @brief Traverses the instance's task tree using createIterator() (DepthFirstIterator)
     *        and returns formatted status lines for FTXUI to render.
     */
    std::vector<std::string> getInstanceTaskTreeStatus(int instanceId) const;

    /**
     * @brief Sets a key-value pair in an instance's runtime DataPayload.
     */
    void setPayloadValue(int instanceId, const std::string &key, const std::string &val);

    /**
     * @brief Returns a copy/reference of the instance's DataPayload map for the TUI inspector.
     */
    std::map<std::string, std::string> getPayloadMap(int instanceId) const;

    /**
     * @brief Returns the activity log vector for the FTXUI bottom console.
     */
    const std::vector<std::string> &getActivityLog() const;
};

#endif // WORKFLOWENGINE_H
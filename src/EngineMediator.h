#ifndef ENGINEMEDIATOR_H
#define ENGINEMEDIATOR_H

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "WorkflowOrchestrator.h"
#include "AssignmentStrategy.h"
#include "DataAdapter.h"
#include "WorkflowComponent.h"
#include "Participant.h"
#include "CommandHistory.h"

class Task;
class DataPayload;

/**
 * @brief How the children of a composite (Subprocess) are run.
 * Sequential: one after another, in the order they were added.
 * Parallel:   all start together; the composite is done when all are done.
 */
enum class ExecutionMode
{
    Sequential,
    Parallel
};

/**
 * @brief Concrete Mediator (GoF role: ConcreteMediator) and Strategy context.
 *
 * Listens for WorkflowEvents from components and decides which component
 * starts next. When a task becomes ready it asks the current
 * AssignmentStrategy who should do it.
 *
 * Ownership: owns the strategy. Does NOT own the component tree or the
 * participants; the caller keeps them alive while the mediator uses them.
 */
class EngineMediator : public WorkflowOrchestrator
{
    typedef void (EngineMediator::*Handler)(WorkflowComponent *);

    WorkflowComponent *root;
    AssignmentStrategy *strategy; // owned
    DataAdapter *adapter;         // owned, loads data from an external system (Adapter pattern)
    std::vector<Participant *> candidates;
    std::vector<WorkflowComponent *> modeKeys; // composites that have a mode set...
    std::vector<ExecutionMode> modeValues;     //...and their modes (same position in both lists)
    Handler handlers[6];                       // one handler per WorkflowEvent, found by the event's number
    CommandHistory history;                    // one history per workflow instance
    DataPayload *branchData;
    bool finished;
    bool halted;
    WorkflowComponent *haltedAt;

    // event handlers (looked up in the handlers table, no switch needed)
    void onStarted(WorkflowComponent *sender);
    void onFinished(WorkflowComponent *sender);
    void onHalt(WorkflowComponent *sender);
    void onEscalated(WorkflowComponent *sender);

    // helpers
    void startComponent(WorkflowComponent *c);
    WorkflowComponent *findParent(WorkflowComponent *node, WorkflowComponent *target) const;
    bool isDone(WorkflowComponent *c) const;
    ExecutionMode modeOf(const WorkflowComponent *c) const;
    static bool stateIs(WorkflowComponent *c, const char *name);
    static void flatten(Participant *p, std::vector<Participant *> &out);
    bool runAndNotify(Command *cmd, WorkflowComponent *sender, WorkflowEvent event);
    WorkflowComponent *findHalted(WorkflowComponent *node) const;
    void refreshStatus();

    // the history owns raw pointers, so the mediator must not be copied
    EngineMediator(const EngineMediator &) = delete;
    EngineMediator &operator=(const EngineMediator &) = delete;

    bool applies(WorkflowComponent *c) const; // branchcondition
public:
    EngineMediator();
    ~EngineMediator();

    // ---- setup ----
    /** Sets the workflow tree and registers this mediator with every component in it.*/
    void setRoot(WorkflowComponent *r);
    /** Registers this mediator with c and all its descendants (call again after adding nodes).*/
    void registerTree(WorkflowComponent *c);
    /** Swaps the assignment rule. Takes ownership. May be changed while running.*/
    void setStrategy(AssignmentStrategy *s);
    /** Sets the adapter used by importData. Takes ownership. A different industry just uses a different adapter.*/
    void setDataAdapter(DataAdapter *a);
    /** People who can be assigned work. Departments are flattened into their members.*/
    void setCandidates(const std::vector<Participant *> &people);
    /** Default is Sequential. Only meaningful for composites.*/
    void setExecutionMode(WorkflowComponent *composite, ExecutionMode m);

    // ---- running ----
    /** Starts the root (and, per the execution modes, whatever is ready first).*/
    void startWorkflow();
    /** Mediator interface: components report events here. */
    void notify(WorkflowComponent *sender, WorkflowEvent event) override;

    // ---- actions: each one runs a command. Returns false if the action was refused ----
    // added for command logic
    bool startTask(WorkflowComponent *c);
    bool completeTask(WorkflowComponent *c);
    bool rejectTask(WorkflowComponent *c);
    bool cancelTask(WorkflowComponent *c);
    bool recordDecision(Task *t, const std::string &outcome);
    bool setData(DataPayload *data, const std::string &key, const std::string &value);
    // Loads the record with this id through the adapter and writes it into data using the standard keys.
    // It is ONE macro command, so the import is one history entry, one undo, all or nothing.
    bool importData(const std::string &id, DataPayload *data);
    bool undo();
    bool redo();
    std::vector<std::string> getHistory() const;

    // ---- decisions (public so they can be tested and reused) ----
    // First child of parent that is not started, provided every earlier child is done.
    WorkflowComponent *findNextReady(WorkflowComponent *parent);
    // Asks the current strategy to pick a performer for c and runs an AssignTaskCommand (no-op if c is not a task).
    void assign(WorkflowComponent *c);

    // ---- status ----
    bool isFinished() const { return finished; }
    bool isHalted() const { return halted; }
    WorkflowComponent *getHaltedAt() const { return haltedAt; }
    const AssignmentStrategy *getStrategy() const { return strategy; }
    void setPayload(DataPayload *d) { branchData = d; } // branchconditions
};

#endif

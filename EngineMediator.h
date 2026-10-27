#ifndef ENGINEMEDIATOR_H
#define ENGINEMEDIATOR_H

#include <map>
#include <memory>
#include <vector>

#include "WorkflowOrchestrator.h"
#include "AssignmentStrategy.h"
#include "WorkflowComponent.h"
#include "Participant.h"

/**
 * @brief How the children of a composite (Subprocess) are run.
 * Sequential: one after another, in the order they were added.
 * Parallel:   all start together; the composite is done when all are done.
 */
enum class ExecutionMode { Sequential, Parallel };

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
class EngineMediator : public WorkflowOrchestrator {
    typedef void (EngineMediator::*Handler)(WorkflowComponent*);

    WorkflowComponent* root;
    std::unique_ptr<AssignmentStrategy> strategy;
    std::vector<Participant*> candidates;
    std::map<const WorkflowComponent*, ExecutionMode> modes;
    std::map<WorkflowEvent, Handler> handlers;

    bool finished;
    bool halted;
    WorkflowComponent* haltedAt;

    // event handlers (looked up in the handlers table, no switch needed)
    void onStarted(WorkflowComponent* sender);
    void onFinished(WorkflowComponent* sender);
    void onHalt(WorkflowComponent* sender);
    void onEscalated(WorkflowComponent* sender);

    // helpers
    void startComponent(WorkflowComponent* c);
    WorkflowComponent* findParent(WorkflowComponent* node, WorkflowComponent* target) const;
    bool isDone(WorkflowComponent* c) const;
    ExecutionMode modeOf(const WorkflowComponent* c) const;
    static bool stateIs(WorkflowComponent* c, const char* name);
    static void flatten(Participant* p, std::vector<Participant*>& out);

public:
    EngineMediator();
    ~EngineMediator();

    // ---- setup ----
    /** Sets the workflow tree and registers this mediator with every component in it. */
    void setRoot(WorkflowComponent* r);
    /** Registers this mediator with c and all its descendants (call again after adding nodes). */
    void registerTree(WorkflowComponent* c);
    /** Swaps the assignment rule. Takes ownership. May be changed while running. */
    void setStrategy(std::unique_ptr<AssignmentStrategy> s);
    /** People who can be assigned work. Departments are flattened into their members. */
    void setCandidates(const std::vector<Participant*>& people);
    /** Default is Sequential. Only meaningful for composites. */
    void setExecutionMode(WorkflowComponent* composite, ExecutionMode m);

    // ---- running ----
    /** Starts the root (and, per the execution modes, whatever is ready first). */
    void startWorkflow();
    /** Mediator interface: components report events here. */
    void notify(WorkflowComponent* sender, WorkflowEvent event) override;

    // ---- decisions (public so they can be tested and reused) ----
    /** First child of parent that is not started, provided every earlier child is done. */
    WorkflowComponent* findNextReady(WorkflowComponent* parent);
    /** Asks the current strategy to pick a performer for c (no-op if c is not a task). */
    void assign(WorkflowComponent* c);

    // ---- status ----
    bool isFinished() const { return finished; }
    bool isHalted() const { return halted; }
    WorkflowComponent* getHaltedAt() const { return haltedAt; }
    const AssignmentStrategy* getStrategy() const { return strategy.get(); }
};

#endif

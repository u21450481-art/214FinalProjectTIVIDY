/**
 * @file testState.cpp
 * @brief Standalone unit test driver for the State pattern workflow lifecycle.
 */

#include <cassert>
#include <iostream>
#include <string>
#include <memory>

void printHeader(const std::string &title)
{
    std::cout << "\n============================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "============================================================\n";
}

// Forward declarations
class WorkflowContext;

// State Interface
class WorkflowState {
public:
    virtual ~WorkflowState() = default;
    virtual std::string getName() const = 0;
    virtual void handleStart(WorkflowContext* context) = 0;
    virtual void handlePause(WorkflowContext* context) = 0;
    virtual void handleComplete(WorkflowContext* context) = 0;
};

// Context class that maintains the current state
class WorkflowContext {
private:
    std::unique_ptr<WorkflowState> currentState;
    std::string lastMessage;

public:
    WorkflowContext(std::unique_ptr<WorkflowState> initialState) 
        : currentState(std::move(initialState)) {}

    void transitionTo(std::unique_ptr<WorkflowState> newState) {
        currentState = std::move(newState);
    }

    std::string getCurrentStateName() const {
        return currentState->getName();
    }

    void requestStart() { currentState->handleStart(this); }
    void requestPause() { currentState->handlePause(this); }
    void requestComplete() { currentState->handleComplete(this); }

    void setMessage(const std::string& msg) { lastMessage = msg; }
    std::string getMessage() const { return lastMessage; }
};

// Concrete States Declarations
class PendingState : public WorkflowState {
public:
    std::string getName() const override { return "Pending"; }
    void handleStart(WorkflowContext* context) override;
    void handlePause(WorkflowContext* context) override {
        context->setMessage("Error: Cannot pause a workflow that hasn't started.");
    }
    void handleComplete(WorkflowContext* context) override {
        context->setMessage("Error: Cannot complete a pending workflow directly.");
    }
};

class RunningState : public WorkflowState {
public:
    std::string getName() const override { return "Running"; }
    void handleStart(WorkflowContext* context) override {
        context->setMessage("Info: Workflow is already running.");
    }
    void handlePause(WorkflowContext* context) override;
    void handleComplete(WorkflowContext* context) override;
};

class PausedState : public WorkflowState {
public:
    std::string getName() const override { return "Paused"; }
    void handleStart(WorkflowContext* context) override;
    void handlePause(WorkflowContext* context) override {
        context->setMessage("Info: Workflow is already paused.");
    }
    void handleComplete(WorkflowContext* context) override;
};

class CompletedState : public WorkflowState {
public:
    std::string getName() const override { return "Completed"; }
    void handleStart(WorkflowContext* context) override {
        context->setMessage("Error: Cannot restart a completed workflow.");
    }
    void handlePause(WorkflowContext* context) override {
        context->setMessage("Error: Cannot pause a completed workflow.");
    }
    void handleComplete(WorkflowContext* context) override {
        context->setMessage("Info: Workflow is already completed.");
    }
};

// State Transition Implementations
void PendingState::handleStart(WorkflowContext* context) {
    context->setMessage("Transitioned from Pending to Running.");
    context->transitionTo(std::make_unique<RunningState>());
}

void RunningState::handlePause(WorkflowContext* context) {
    context->setMessage("Transitioned from Running to Paused.");
    context->transitionTo(std::make_unique<PausedState>());
}

void RunningState::handleComplete(WorkflowContext* context) {
    context->setMessage("Transitioned from Running to Completed.");
    context->transitionTo(std::make_unique<CompletedState>());
}

void PausedState::handleStart(WorkflowContext* context) {
    context->setMessage("Transitioned from Paused back to Running.");
    context->transitionTo(std::make_unique<RunningState>());
}

void PausedState::handleComplete(WorkflowContext* context) {
    context->setMessage("Transitioned from Paused to Completed.");
    context->transitionTo(std::make_unique<CompletedState>());
}

int main()
{
    printHeader("SETUP: Initializing Workflow Context in Pending State");
    WorkflowContext workflow(std::make_unique<PendingState>());
    assert(workflow.getCurrentStateName() == "Pending");
    std::cout << "[PASS] Initial state verified as Pending.\n";

    // TEST 1: Invalid action on Pending state (Trying to pause)
    {
        printHeader("TEST 1: Invalid Action on Pending State");
        workflow.requestPause();
        std::cout << "[LOG] " << workflow.getMessage() << "\n";
        assert(workflow.getCurrentStateName() == "Pending");
        std::cout << "[PASS] Pending state correctly blocked pause action.\n";
    }

    // TEST 2: Valid Transition: Pending -> Running
    {
        printHeader("TEST 2: Transition Pending -> Running");
        workflow.requestStart();
        std::cout << "[LOG] " << workflow.getMessage() << "\n";
        assert(workflow.getCurrentStateName() == "Running");
        std::cout << "[PASS] Successfully transitioned to Running state!\n";
    }

    // TEST 3: Valid Transition: Running -> Paused
    {
        printHeader("TEST 3: Transition Running -> Paused");
        workflow.requestPause();
        std::cout << "[LOG] " << workflow.getMessage() << "\n";
        assert(workflow.getCurrentStateName() == "Paused");
        std::cout << "[PASS] Successfully transitioned to Paused state!\n";
    }

    // TEST 4: Resume from Paused: Paused -> Running
    {
        printHeader("TEST 4: Resume from Paused -> Running");
        workflow.requestStart();
        std::cout << "[LOG] " << workflow.getMessage() << "\n";
        assert(workflow.getCurrentStateName() == "Running");
        std::cout << "[PASS] Successfully resumed back to Running state!\n";
    }

    // TEST 5: Complete Workflow: Running -> Completed
    {
        printHeader("TEST 5: Transition Running -> Completed");
        workflow.requestComplete();
        std::cout << "[LOG] " << workflow.getMessage() << "\n";
        assert(workflow.getCurrentStateName() == "Completed");
        std::cout << "[PASS] Workflow successfully completed!\n";
    }

    // TEST 6: Invalid action on Completed state (Trying to restart)
    {
        printHeader("TEST 6: Invalid Action on Completed State");
        workflow.requestStart();
        std::cout << "[LOG] " << workflow.getMessage() << "\n";
        assert(workflow.getCurrentStateName() == "Completed");
        std::cout << "[PASS] Completed state correctly blocked restart action.\n";
    }

    printHeader("ALL STATE PATTERN TESTS PASSED SUCCESSFULLY!");
    return 0;
}
#ifndef CONCRETE_STAGES_H
#define CONCRETE_STAGES_H

#include "Stage.h"

/**
 * @file ConcreteStages.h
 * @brief Defines the concrete states for the workflow component State pattern.
 */

/**
 * @brief Represents the initial NotStarted state of a workflow component.
 */
class NotStarted : public Stage{
public:
    /**
     * @brief Handles execution when the component is in the NotStarted state.
     * @param c Pointer to the WorkflowComponent being executed.
     */
    void handleExecute(WorkflowComponent* c) override;

    /**
     * @brief Transitions the component from NotStarted to InProgress.
     * @param c Pointer to the WorkflowComponent.
     */
    void start(WorkflowComponent* c) override;

    /**
     * @brief Cancels the workflow component from the NotStarted state.
     * @param c Pointer to the WorkflowComponent.
     */
    void cancel(WorkflowComponent* c) override;

    /**
     * @brief Gets the string name of the state.
     * @return std::string representing the state name ("NotStarted").
     */
    std::string getName() const override {return "NotStarted";}
};

/**
 * @class InProgress
 * @brief Represents the active state while a workflow component is currently executing.
 */
class InProgress : public Stage{
public:
    /**
     * @brief Handles execution while in the InProgress state.
     * @param c Pointer to the WorkflowComponent.
     */
    void handleExecute(WorkflowComponent* c) override;

    /**
     * @brief Completes the workflow component execution.
     * @param c Pointer to the WorkflowComponent.
     */
    void complete(WorkflowComponent* c) override;

    /**
     * @brief Rejects the workflow component execution.
     * @param c Pointer to the WorkflowComponent.
     */
    void reject(WorkflowComponent* c) override;

    /**
     * @brief Cancels the workflow component execution.
     * @param c Pointer to the WorkflowComponent.
     */
    void cancel(WorkflowComponent* c) override;

    /**
     * @brief Gets the string name of the state.
     * @return std::string representing the state name ("InProgress").
     */
    std::string getName() const override {return "InProgress";}
};

/**
 * @class Complete
 * @brief Represents the terminal success state of a workflow component.
 */
class Complete : public Stage{
public:
    /**
     * @brief Handles execution when already complete (execution blocked).
     * @param c Pointer to the WorkflowComponent.
     */
    void handleExecute(WorkflowComponent* c) override;

    /**
     * @brief Gets the string name of the state.
     * @return std::string representing the state name ("Complete").
     */
    std::string getName() const override {return "Complete";}
};

/**
 * @class Rejected
 * @brief Represents the terminal rejected state of a workflow component.
 */
class Rejected : public Stage{
public:
    /**
     * @brief Handles execution when the workflow component is rejected.
     * @param c Pointer to the WorkflowComponent.
     */
    void handleExecute(WorkflowComponent* c) override;

    /**
     * @brief Gets the string name of the state.
     * @return std::string representing the state name ("Rejected").
     */
    std::string getName() const override {return "Rejected";}
};

/**
 * @class Cancelled
 * @brief Represents the terminal cancelled state of a workflow component.
 */
class Cancelled : public Stage{
public:
    /**
     * @brief Handles execution when the workflow component is cancelled.
     * @param c Pointer to the WorkflowComponent.
     */
    void handleExecute(WorkflowComponent* c) override;

    /**
     * @brief Gets the string name of the state.
     * @return std::string representing the state name ("Cancelled").
     */
        std::string getName() const override {return "Cancelled";}
};

#endif


/**
 * @file WorkflowInstance.h
 * @brief Active runtime execution instance spawned from a WorkflowDefinition.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - Role: Active Runtime Instance (Spawned by Builder Product / WorkflowDefinition)
 * - Pattern Connections:
 *   1. Builder / Prototype: Created when WorkflowDefinition::instantiate() clones
 *      the static template's task tree into an isolated running instance.
 *   2. Composition Owner of DataPayload: Holds a dedicated DataPayload object
 *      so concurrent workflow instances never overwrite each other's data.
 *   3. Mediator Context: Connects the active root WorkflowComponent tree to the
 *      WorkflowOrchestrator (EngineMediator) for runtime event routing.
 * ============================================================================
 */

#ifndef WORKFLOWINSTANCE_H
#define WORKFLOWINSTANCE_H

#include <string>
#include "DataPayload.h"

// Forward declarations of classes from other patterns
class WorkflowComponent;     // Composite Pattern (Component)
class WorkflowOrchestrator;  // Mediator Pattern (Mediator interface)
class EngineMediator;        // Mediator Pattern (ConcreteMediator)
class Participant;           // Composite Pattern (Participant tree)

/**
 * @class WorkflowInstance
 * @brief Represents one independent execution of a workflow definition with its own data and state.
 */
class WorkflowInstance {
private:
    /** @brief Pointer to the cloned root of the active WorkflowComponent tree. */
    WorkflowComponent* root;

    /** @brief Unique identifier for this running workflow instance. */
    int id;

    /** @brief Name of the workflow definition template this instance was spawned from. */
    std::string definitionName;

    /** @brief Pointer to the mediator coordinating task execution and routing. */
    WorkflowOrchestrator* mediator;

    /** @brief Version number of the WorkflowDefinition used to spawn this instance. */
    int definitionVersion;

    /** @brief Composed runtime data payload unique to this workflow instance. */
    DataPayload payload;

public:
    /**
     * @brief Constructs a new WorkflowInstance.
     * @param instanceId Unique ID for this run.
     * @param defName Name of the source WorkflowDefinition.
     * @param defVersion Version of the source WorkflowDefinition.
     * @param clonedRoot Root pointer of the cloned WorkflowComponent composite tree.
     * @param orchestrator Pointer to the mediator managing this instance (optional at init).
     */
    WorkflowInstance(int instanceId,
                     const std::string& defName,
                     int defVersion,
                     WorkflowComponent* clonedRoot = nullptr,
                     WorkflowOrchestrator* orchestrator = nullptr);

    /**
     * @brief Destructor. Cleans up the cloned task tree owned by this instance.
     */
    ~WorkflowInstance();

    /**
     * @brief Starts execution of the workflow instance from the root component.
     */
    void start();

    /**
     * @brief Stops/halts execution of the workflow instance.
     */
    void stop();

    /**
     * @brief Provides mutable access to the instance's DataPayload.
     * @return Reference to the composed DataPayload.
     */
    DataPayload& getPayload();

    /**
     * @brief Provides read-only access to the instance's DataPayload.
     * @return Const reference to the composed DataPayload.
     */
    const DataPayload& getPayload() const;

    // --- Getters & Setters for Engine & Mediator coordination ---
    int getId() const;
    std::string getDefinitionName() const;
    int getDefinitionVersion() const;
    WorkflowComponent* getRoot() const;
    void setRoot(WorkflowComponent* newRoot);
    WorkflowOrchestrator* getMediator() const;
    void setMediator(WorkflowOrchestrator* newMediator);
};

#endif // WORKFLOWINSTANCE_H

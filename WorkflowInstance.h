/**
 * @file WorkflowInstance.h
 * @brief One independent run of a WorkflowDefinition.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - Role: Active runtime instance, created by WorkflowDefinition::instantiate()
 * - Pattern Connections:
 *   1. Prototype: owns the cloned copy of the WorkflowDefinition's task tree.
 *   2. Composition: owns a private DataPayload, so concurrent instances
 *      never overwrite each other's data.
 *   3. Mediator context: holds a non-owning pointer to the mediator that
 *      coordinates this instance's components.
 * ============================================================================
 */

#ifndef WORKFLOWINSTANCE_H
#define WORKFLOWINSTANCE_H

#include <string>

#include "DataPayload.h"

class WorkflowComponent;    ///< Composite pattern (Component)
class WorkflowOrchestrator; ///< Mediator pattern (Mediator interface)

/**
 * @class WorkflowInstance
 * @brief One execution of a WorkflowDefinition, with its own task tree, data and ID.
 *
 * Ownership: the instance owns its task tree and payload. It does NOT own
 * the mediator. It cannot be copied.
 */
class WorkflowInstance
{
private:
    /** @brief Root of this instance's cloned task tree (owned). */
    WorkflowComponent *root;

    /** @brief Unique ID of this instance. */
    int id;

    /** @brief Name of the WorkflowDefinition this instance was spawned from. */
    std::string definitionName;

    /** @brief Mediator coordinating this instance (not owned, may be null). */
    WorkflowOrchestrator *mediator;

    /** @brief Version of the WorkflowDefinition this instance was spawned from. */
    int definitionVersion;

    /** @brief Runtime data unique to this instance. */
    DataPayload payload;

public:
    /**
     * @brief Constructs an instance.
     * @param instanceId Unique ID for this run.
     * @param defName Name of the source WorkflowDefinition.
     * @param defVersion Version of the source WorkflowDefinition.
     * @param clonedRoot Root of the cloned task tree; the instance takes ownership.
     * @param orchestrator Mediator for this instance (optional, not owned).
     */
    WorkflowInstance(int instanceId,
                     const std::string &defName,
                     int defVersion,
                     WorkflowComponent *clonedRoot = nullptr,
                     WorkflowOrchestrator *orchestrator = nullptr);

    /**
     * @brief Destroys the instance, deleting its task tree.
     * The mediator is not deleted.
     */
    ~WorkflowInstance();

    /** @brief Copying is disabled: the instance owns a raw task tree. */
    WorkflowInstance(const WorkflowInstance &) = delete;

    /** @brief Copy assignment is disabled: the instance owns a raw task tree. */
    WorkflowInstance &operator=(const WorkflowInstance &) = delete;

    /**
     * @brief Starts the root component directly.
     * @note This bypasses the mediator and the command history. Use
     *       EngineMediator::startWorkflow() for normal execution.
     */
    void start();

    /**
     * @brief Cancels the root component directly.
     * @note This bypasses the mediator and the command history.
     */
    void stop();

    /**
     * @brief Gets mutable access to this instance's payload.
     * @return Reference to the payload.
     */
    DataPayload &getPayload();

    /**
     * @brief Gets read-only access to this instance's payload.
     * @return Const reference to the payload.
     */
    const DataPayload &getPayload() const;

    /**
     * @brief Gets the instance ID.
     * @return The unique ID.
     */
    int getId() const;

    /**
     * @brief Gets the name of the source WorkflowDefinition.
     * @return The WorkflowDefinition name.
     */
    const std::string &getDefinitionName() const;

    /**
     * @brief Gets the version of the source WorkflowDefinition.
     * @return The WorkflowDefinition version.
     */
    int getDefinitionVersion() const;

    /**
     * @brief Gets the root of this instance's task tree.
     * @return Non-owning pointer, or nullptr if there is no tree.
     */
    WorkflowComponent *getRoot() const;

    /**
     * @brief Gets the mediator coordinating this instance.
     * @return Non-owning pointer, or nullptr if none is attached.
     */
    WorkflowOrchestrator *getMediator() const;

    /**
     * @brief Attaches a mediator to this instance.
     * @param newMediator The mediator (not owned; it must outlive this instance's use of it).
     */
    void setMediator(WorkflowOrchestrator *newMediator);
};

#endif // WORKFLOWINSTANCE_H
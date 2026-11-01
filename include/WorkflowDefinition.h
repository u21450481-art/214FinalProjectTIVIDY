/**
 * @file WorkflowDefinition.h
 * @brief Master WorkflowDefinition assembled by the Builder pattern.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - GoF Pattern: Builder Pattern (Participant: Product)
 * - Secondary Pattern Role: Prototype client (via instantiate())
 * - Role Explanation:
 *   WorkflowDefinition is the complex object (Product) assembled step by step
 *   by a WorkflowDefinitionBuilder. It is a reusable, static definition holding
 *   the root of the task tree, the registered participants and version
 *   metadata. instantiate() clones the task tree into a new, independent
 *   WorkflowInstance, so running a workflow never changes the definition.
 * ============================================================================
 */

#ifndef WORKFLOWDEFINITION_H
#define WORKFLOWDEFINITION_H

#include <memory>
#include <string>
#include <vector>

#include "WorkflowInstance.h"

class WorkflowComponent; ///< Composite pattern: Task/Subprocess tree
class Participant;       ///< Composite pattern: Employee/Department tree

/**
 * @class WorkflowDefinition
 * @brief The static master template of a workflow, built by a ConcreteBuilder.
 *
 * Ownership: the definition owns its root task tree and all top-level
 * participants and deletes them in its destructor. It cannot be copied,
 * because a copy would delete the same objects twice.
 */
class WorkflowDefinition
{
private:
    /** @brief Version number of this WorkflowDefinition. */
    int version;

    /** @brief Human-readable WorkflowDefinition name, e.g. "Retail_Return_Item". */
    std::string name;

    /** @brief Root of the master task tree (owned). */
    WorkflowComponent *root;

    /** @brief Top-level participants (owned). Employees inside a Department are owned by that Department. */
    std::vector<Participant *> participants;

public:
    /**
     * @brief Constructs an empty WorkflowDefinition.
     * @param defName Name of the workflow definition.
     * @param ver Version number (defaults to 1).
     */
    explicit WorkflowDefinition(const std::string &defName = "Unnamed_Workflow", int ver = 1);

    /**
     * @brief Destroys the WorkflowDefinition, deleting the task tree and all top-level participants.
     */
    ~WorkflowDefinition();

    /** @brief Copying is disabled: the definition owns raw pointers. */
    WorkflowDefinition(const WorkflowDefinition &) = delete;

    /** @brief Copy assignment is disabled: the definition owns raw pointers. */
    WorkflowDefinition &operator=(const WorkflowDefinition &) = delete;

    /**
     * @brief Spawns a fresh, independent WorkflowInstance from this WorkflowDefinition.
     *
     * Uses the Prototype pattern: the root task tree is cloned with
     * WorkflowComponent::clone(), so changes to the instance never affect
     * this WorkflowDefinition or other instances.
     *
     * @param instanceId Unique ID chosen by the caller (the WorkflowEngine).
     * @return The new instance, which owns its cloned task tree.
     */
    std::unique_ptr<WorkflowInstance> instantiate(int instanceId);

    /**
     * @brief Gets the WorkflowDefinition version.
     * @return The version number.
     */
    int getVersion() const;

    /**
     * @brief Gets the WorkflowDefinition name.
     * @return The name (the engine uses it as the registry key).
     */
    const std::string &getName() const;

    /**
     * @brief Sets the root of the master task tree.
     *
     * Takes ownership of @p rootComponent. Any previously set root is
     * deleted, so replacing the root does not leak.
     *
     * @param rootComponent The new root (usually a Subprocess).
     */
    void setRoot(WorkflowComponent *rootComponent);

    /**
     * @brief Gets the root of the master task tree.
     * @return Non-owning pointer to the root, or nullptr if none is set.
     */
    WorkflowComponent *getRoot() const;

    /**
     * @brief Registers a top-level participant (Employee or Department).
     *
     * Takes ownership of @p participant. A null pointer is ignored.
     *
     * @param participant The participant to register.
     */
    void addParticipant(Participant *participant);

    /**
     * @brief Gets the top-level participants.
     *
     * Employees inside a Department are not listed here; reach them through
     * Participant::getChildren() on the Department.
     *
     * @return Const reference to the list (the definition keeps ownership).
     */
    const std::vector<Participant *> &getParticipants() const;
};

#endif // WORKFLOWDEFINITION_H
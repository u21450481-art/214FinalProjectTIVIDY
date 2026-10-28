/**
 * @file WorkflowDefinition.h
 * @brief Master blueprint constructed by the Builder pattern.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - GoF Pattern: Builder Pattern (Participant: Product)
 * - Secondary Pattern Role: Prototype Spawner (via instantiate())
 * - Role Explanation:
 *   WorkflowDefinition is the complex object (Product) assembled step-by-step
 *   by the WorkflowDefinitionBuilder hierarchy. It represents a reusable,
 *   static master blueprint containing the root of the task tree, registered
 *   participants, and version metadata. Calling instantiate() clones the
 *   blueprint's task tree into a brand-new, independent WorkflowInstance so
 *   executing a workflow never mutates or corrupts the master blueprint.
 * ============================================================================
 */

#ifndef WORKFLOWDEFINITION_H
#define WORKFLOWDEFINITION_H

#include <string>
#include <vector>
#include <memory>
#include "WorkflowInstance.h"

// Forward declarations for other pattern classes
class WorkflowComponent;     // Composite Pattern (Task/Subprocess tree)
class Participant;           // Composite Pattern (Employee/Department tree)
class WorkflowOrchestrator;  // Mediator Pattern

/**
 * @class WorkflowDefinition
 * @brief Represents the static master template of a workflow built by a ConcreteBuilder.
 */
class WorkflowDefinition {
private:
    /** @brief Version number of this workflow blueprint. */
    int version;

    /** @brief Human-readable name of this workflow definition (e.g., "Retail_Onboarding"). */
    std::string name;

    /**
     * @brief Pointer to the root of the master Composite task tree.
     * Note: Stored as a raw pointer in the skeleton so it compiles with forward
     * declarations before WorkflowComponent.h is created; can be wrapped in
     * std::unique_ptr<WorkflowComponent> once WorkflowComponent.h is included.
     */
    WorkflowComponent* root;

    /** @brief Collection of participants/departments built for this workflow definition. */
    std::vector<Participant*> participants;

    /** @brief Counter used to generate unique IDs for spawned WorkflowInstances. */
    int nextInstanceId;

public:
    /**
     * @brief Constructs an empty WorkflowDefinition product.
     * @param defName Name of the workflow definition.
     * @param ver Version number (defaults to 1).
     */
    WorkflowDefinition(const std::string& defName = "Unnamed_Workflow", int ver = 1);

    /**
     * @brief Destructor. Cleans up the master blueprint's root task tree.
     */
    ~WorkflowDefinition();

    /**
     * @brief Spawns a fresh, independent WorkflowInstance from this master blueprint.
     * Uses the Prototype pattern (WorkflowComponent::clone()) on the root task tree
     * so changes in the running instance never affect this definition or other instances.
     * @return std::unique_ptr<WorkflowInstance> owning the newly created instance.
     */
    std::unique_ptr<WorkflowInstance> instantiate();

    // --- Builder assembly methods (used by ConcreteBuilders) ---

    /**
     * @brief Sets the version number of the workflow blueprint.
     * @param ver Version integer.
     */
    void setVersion(int ver);

    /**
     * @brief Gets the version number of the workflow blueprint.
     * @return Version integer.
     */
    int getVersion() const;

    /**
     * @brief Sets the name of the workflow definition.
     * @param defName Blueprint name string.
     */
    void setName(const std::string& defName);

    /**
     * @brief Gets the name of the workflow definition.
     * @return Blueprint name string.
     */
    std::string getName() const;

    /**
     * @brief Sets the root component of the master task tree.
     * @param rootComponent Pointer to the root WorkflowComponent (usually a Subprocess).
     */
    void setRoot(WorkflowComponent* rootComponent);

    /**
     * @brief Gets the root component of the master task tree.
     * @return Pointer to the root WorkflowComponent.
     */
    WorkflowComponent* getRoot() const;

    /**
     * @brief Registers a Participant (Employee or Department) in the definition.
     * @param participant Pointer to the Participant to add.
     */
    void addParticipant(Participant* participant);

    /**
     * @brief Returns the list of participants associated with this definition.
     * @return Const reference to the vector of Participant pointers.
     */
    const std::vector<Participant*>& getParticipants() const;
};

#endif // WORKFLOWDEFINITION_H

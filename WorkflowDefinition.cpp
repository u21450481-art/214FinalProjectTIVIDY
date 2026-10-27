/**
 * @file WorkflowDefinition.cpp
 * @brief Skeleton implementation of WorkflowDefinition (Builder Product).
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - GoF Pattern: Builder Pattern (Participant: Product)
 * - Assembled by WorkflowDefinitionBuilder and ConcreteBuilders.
 * - Spawns isolated WorkflowInstance objects via instantiate().
 * ============================================================================
 */

#include "WorkflowDefinition.h"
#include <iostream>

#include "WorkflowComponent.h"
#include "Participant.h"

WorkflowDefinition::WorkflowDefinition(const std::string &defName, int ver)
    : version(ver),
      name(defName),
      root(nullptr),
      participants(),
      nextInstanceId(1) {}

WorkflowDefinition::~WorkflowDefinition()
{
    // MEMORY LEAK
    // delete the root task tree (this will recursively delete all Tasks and Subprocesses)
    if (root != nullptr)
    {
        delete root;
        root = nullptr;
    }

    // delete all registered participants (Departments and Employees)
    for (Participant *p : participants)
    {
        delete p;
    }
    participants.clear();
}

std::unique_ptr<WorkflowInstance> WorkflowDefinition::instantiate()
{
    WorkflowComponent *clonedRoot = nullptr;

    if (root != nullptr)
    {
        clonedRoot = root->clone();
    }

    int assignedId = nextInstanceId++;
    std::cout << "[WorkflowDefinition] Instantiating new WorkflowInstance #"
              << assignedId << " from blueprint '" << name
              << "' (v" << version << ")." << std::endl;

    return std::unique_ptr<WorkflowInstance>(
        new WorkflowInstance(assignedId, name, version, clonedRoot, nullptr));
}

void WorkflowDefinition::setVersion(int ver)
{
    version = ver;
}

int WorkflowDefinition::getVersion() const
{
    return version;
}

void WorkflowDefinition::setName(const std::string &defName)
{
    name = defName;
}

std::string WorkflowDefinition::getName() const
{
    return name;
}

void WorkflowDefinition::setRoot(WorkflowComponent *rootComponent)
{
    root = rootComponent;
}

WorkflowComponent *WorkflowDefinition::getRoot() const
{
    return root;
}

void WorkflowDefinition::addParticipant(Participant *participant)
{
    if (participant != nullptr)
    {
        participants.push_back(participant);
    }
}

const std::vector<Participant *> &WorkflowDefinition::getParticipants() const
{
    return participants;
}

/**
 * @file WorkflowDefinition.cpp
 * @brief Implementation of WorkflowDefinition (Builder Product).
 */

#include "WorkflowDefinition.h"

#include "Log.h"
#include "Participant.h"
#include "WorkflowComponent.h"

WorkflowDefinition::WorkflowDefinition(const std::string &defName, int ver)
    : version(ver),
      name(defName),
      root(nullptr),
      participants() {}

WorkflowDefinition::~WorkflowDefinition()
{
    // Deleting the root recursively deletes every Subprocess and Task below it.
    delete root;
    root = nullptr;

    // Deleting a Department also deletes the Employees inside it.
    for (Participant *p : participants)
    {
        delete p;
    }
    participants.clear();
}

std::unique_ptr<WorkflowInstance> WorkflowDefinition::instantiate(int instanceId)
{
    // Prototype: clone the whole task tree so the instance is isolated.
    WorkflowComponent *clonedRoot = (root != nullptr) ? root->clone() : nullptr;

    TLOG("[WorkflowDefinition] Instantiating #" << instanceId
                                                << " from WorkflowDefinition '" << name << "' (v" << version << ")");

    return std::unique_ptr<WorkflowInstance>(
        new WorkflowInstance(instanceId, name, version, clonedRoot, nullptr));
}

int WorkflowDefinition::getVersion() const
{
    return version;
}

const std::string &WorkflowDefinition::getName() const
{
    return name;
}

void WorkflowDefinition::setRoot(WorkflowComponent *rootComponent)
{
    if (root != rootComponent)
    {
        delete root; // avoid leaking the old root
        root = rootComponent;
    }
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
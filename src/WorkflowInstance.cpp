/**
 * @file WorkflowInstance.cpp
 * @brief Implementation of WorkflowInstance.
 */

#include "WorkflowInstance.h"

#include "Log.h"
#include "WorkflowComponent.h"

WorkflowInstance::WorkflowInstance(int instanceId,
                                   const std::string &defName,
                                   int defVersion,
                                   WorkflowComponent *clonedRoot,
                                   WorkflowOrchestrator *orchestrator)
    : root(clonedRoot),
      id(instanceId),
      definitionName(defName),
      mediator(orchestrator),
      definitionVersion(defVersion),
      payload() {}

WorkflowInstance::~WorkflowInstance()
{
    // Recursively deletes every Subprocess and Task in this instance.
    delete root;
    root = nullptr;
}

void WorkflowInstance::start()
{
    TLOG("[WorkflowInstance #" << id << " - " << definitionName
                               << " v" << definitionVersion << "] Starting...");

    if (root != nullptr)
    {
        root->start();
        root->execute();
    }
}

void WorkflowInstance::stop()
{
    TLOG("[WorkflowInstance #" << id << " - " << definitionName << "] Stopping...");

    if (root != nullptr)
    {
        root->cancel();
    }
}

DataPayload &WorkflowInstance::getPayload()
{
    return payload;
}

const DataPayload &WorkflowInstance::getPayload() const
{
    return payload;
}

int WorkflowInstance::getId() const
{
    return id;
}

const std::string &WorkflowInstance::getDefinitionName() const
{
    return definitionName;
}

int WorkflowInstance::getDefinitionVersion() const
{
    return definitionVersion;
}

WorkflowComponent *WorkflowInstance::getRoot() const
{
    return root;
}

WorkflowOrchestrator *WorkflowInstance::getMediator() const
{
    return mediator;
}

void WorkflowInstance::setMediator(WorkflowOrchestrator *newMediator)
{
    mediator = newMediator;
}
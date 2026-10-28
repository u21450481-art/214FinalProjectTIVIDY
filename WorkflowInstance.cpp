/**
 * @file WorkflowInstance.cpp
 * @brief Skeleton implementation of WorkflowInstance.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - Role: Active Runtime Instance (Spawned by WorkflowDefinition::instantiate())
 * - Owns its cloned WorkflowComponent tree and DataPayload to guarantee
 *   strict instance isolation as required by the TIVIDY specification.
 * ============================================================================
 */

#include "WorkflowInstance.h"
#include <iostream>

#include "WorkflowComponent.h"
#include "WorkflowOrchestrator.h"

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

    if (root != nullptr)
    {
        delete root;
        root = nullptr;
    }
}

void WorkflowInstance::start()
{
    std::cout << "[WorkflowInstance #" << id << " - " << definitionName
              << " v" << definitionVersion << "] Starting execution..." << std::endl;

    if (root != nullptr)
    {
        root->start();
        root->execute();
    }
}

void WorkflowInstance::stop()
{
    std::cout << "[WorkflowInstance #" << id << " - " << definitionName
              << "] Stopping execution..." << std::endl;

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

std::string WorkflowInstance::getDefinitionName() const
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

void WorkflowInstance::setRoot(WorkflowComponent *newRoot)
{
    root = newRoot;
}

WorkflowOrchestrator *WorkflowInstance::getMediator() const
{
    return mediator;
}

void WorkflowInstance::setMediator(WorkflowOrchestrator *newMediator)
{
    mediator = newMediator;
}

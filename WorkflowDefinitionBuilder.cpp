/**
 * @file WorkflowDefinitionBuilder.cpp
 * @brief Base implementation of the abstract WorkflowDefinitionBuilder.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - GoF Pattern: Builder Pattern (Participant: Builder)
 * - Implements default reset() and safe hand-off getResult() semantics so
 *   the constructed WorkflowDefinition survives the builder's lifecycle.
 * ============================================================================
 */

#include "WorkflowDefinitionBuilder.h"

WorkflowDefinitionBuilder::WorkflowDefinitionBuilder() : definition(nullptr) {}

WorkflowDefinitionBuilder::~WorkflowDefinitionBuilder()
{
    // Only delete if a partial build was abandoned before getResult() was called
    delete definition;
    definition = nullptr;
}

void WorkflowDefinitionBuilder::reset()
{
    delete definition;
    definition = new WorkflowDefinition();
}

WorkflowDefinition *WorkflowDefinitionBuilder::getResult()
{
    WorkflowDefinition *result = definition;
    // detach ownership so the product survives the Builder
    definition = nullptr;
    return result;
}

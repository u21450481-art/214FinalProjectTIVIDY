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
    delete definition; // only non-null if a build was abandoned
}

WorkflowDefinition *WorkflowDefinitionBuilder::getResult()
{
    WorkflowDefinition *result = definition;
    definition = nullptr; // ownership leaves the builder
    return result;
}
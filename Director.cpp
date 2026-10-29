/**
 * @file Director.cpp
 * @brief Implementation of the Director class in the Builder pattern.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - GoF Pattern: Builder Pattern (Participant: Director)
 * - Coordinates the construction steps on the abstract WorkflowDefinitionBuilder.
 * ============================================================================
 */

#include "Director.h"
#include <iostream>

Director::Director(WorkflowDefinitionBuilder *b) : builder(b) {}

Director::~Director()
{
    // Aggregation only: Director does not own the builder's memory
    builder = nullptr;
}

void Director::setBuilder(WorkflowDefinitionBuilder *b)
{
    builder = b;
}

void Director::construct()
{
    if (builder == nullptr)
    {
        std::cerr << "[Director] Error: Cannot Construct() because no builder is set!" << std::endl;
        return;
    }
    std::cout << "[Director] Directing step-by-step workflow definition build..." << std::endl;
    builder->reset();
    builder->buildParticipants();
    builder->buildTaskTree();
}

/**
 * @file OnboardingDefinitionBuilder.cpp
 * @brief Skeleton implementation of OnboardingDefinitionBuilder (ConcreteBuilder).
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - GoF Pattern: Builder Pattern (Participant: ConcreteBuilder)
 * - Assembles the concrete Composite Task tree and Participant tree for the
 *   Onboarding scenario.
 * ============================================================================
 */

#include "OnboardingDefinitionBuilder.h"
#include <iostream>

// uncomment as headers are made:
// #include "Task.h"
// #include "Subprocess.h"
// #include "Employee.h"
// #include "Department.h"
// #include "EmailAdapter.h"
// #include "NotStarted.h"

OnboardingDefinitionBuilder::OnboardingDefinitionBuilder()
    : WorkflowDefinitionBuilder()
{
    reset();
}

OnboardingDefinitionBuilder::~OnboardingDefinitionBuilder() {}

void OnboardingDefinitionBuilder::reset()
{
    delete definition;
    definition = new WorkflowDefinition("Retail_Employee_Onboarding", 1);
}

void OnboardingDefinitionBuilder::buildParticipants()
{
    if (definition == nullptr)
    {
        reset();
    }
    std::cout << "[OnboardingDefinitionBuilder] Building onboarding participants "
              << "(HR Department, IT Support, Store Manager)..." << std::endl;

    // complete once composide done
    //  Department* hrDept = new Department("HR Department");
}

void OnboardingDefinitionBuilder::buildTaskTree()
{
    if (definition == nullptr)
    {
        reset();
    }
    std::cout << "[OnboardingDefinitionBuilder] Building onboarding composite task tree "
              << "(HR Paperwork -> IT Setup Subprocess -> Final Approval)..." << std::endl;

    // complete once composide done
    //  Subprocess* rootProcess = new Subprocess("Onboarding Master Process");
}

WorkflowDefinition *OnboardingDefinitionBuilder::getResult()
{
    std::cout << "[OnboardingDefinitionBuilder] Handing off completed WorkflowDefinition '"
              << (definition ? definition->getName() : "null") << "'." << std::endl;
    return WorkflowDefinitionBuilder::getResult();
}

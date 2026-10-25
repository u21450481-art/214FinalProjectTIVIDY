/**
 * @file CustomDefinitionBuilder.cpp
 * @brief Implementation of CustomDefinitionBuilder.
 */

#include "CustomDefinitionBuilder.h"
#include <iostream>

#include "Subprocess.h"
#include "Task.h"
#include "Department.h"
#include "Employee.h"

CustomDefinitionBuilder::CustomDefinitionBuilder() : WorkflowDefinitionBuilder()
{
    reset();
}

CustomDefinitionBuilder::~CustomDefinitionBuilder()
{
    subprocessRegistry.clear();
    departmentRegistry.clear();
}

void CustomDefinitionBuilder::reset()
{
    reset("Custom_Workflow", 1);
}

void CustomDefinitionBuilder::reset(const std::string &workflowName, int version)
{
    delete definition;
    definition = new WorkflowDefinition(workflowName, version);
    subprocessRegistry.clear();
    departmentRegistry.clear();

    std::cout << "[CustomDefinitionBuilder] Initialized blueprint '"
              << workflowName << "' (v" << version << ") with Root Subprocess." << std::endl;

    Subprocess *root = new Subprocess(workflowName + "_Root");
    definition->setRoot(root);
    subprocessRegistry["ROOT"] = root;
}

void CustomDefinitionBuilder::buildParticipants()
{
    // Kept for GoF interface compliance when used with a standard Director
    if (definition == nullptr)
        reset();
}

void CustomDefinitionBuilder::buildTaskTree()
{
    // Kept for GoF interface compliance when used with a standard Director
    if (definition == nullptr)
        reset();
}

void CustomDefinitionBuilder::addDepartment(const std::string &deptName)
{
    if (definition == nullptr)
        reset();
    std::cout << "  + [Builder] Added Department: " << deptName << std::endl;

    Department *dept = new Department(deptName);
    departmentRegistry[deptName] = dept;
    definition->addParticipant(dept);
}

void CustomDefinitionBuilder::addEmployee(const std::string &empName, const std::string &deptName)
{
    if (definition == nullptr)
        reset();
    std::cout << "  + [Builder] Added Employee: " << empName
              << (deptName.empty() ? " (Standalone)" : " -> Dept: " + deptName) << std::endl;

    Employee *emp = new Employee(empName);
    if (!deptName.empty() && departmentRegistry.count(deptName))
    {
        departmentRegistry[deptName]->Add(emp);
    }
    else
    {
        definition->addParticipant(emp);
    }
}

void CustomDefinitionBuilder::addSubprocess(const std::string &subprocessName, const std::string &parentSubprocess)
{
    if (definition == nullptr)
        reset();
    std::string targetParent = parentSubprocess.empty() ? "ROOT" : parentSubprocess;
    std::cout << "  + [Builder] Added Subprocess '" << subprocessName
              << "' inside '" << targetParent << "'" << std::endl;

    Subprocess *sub = new Subprocess(subprocessName);
    subprocessRegistry[subprocessName] = sub;
    if (subprocessRegistry.count(targetParent))
    {
        subprocessRegistry[targetParent]->Add(sub);
    }
}

void CustomDefinitionBuilder::addTask(const std::string &taskName,
                                      const std::string &parentSubprocess,
                                      int priority,
                                      const std::string &branchCondition)
{
    if (definition == nullptr)
        reset();
    std::string targetParent = parentSubprocess.empty() ? "ROOT" : parentSubprocess;
    std::cout << "  + [Builder] Added Task '" << taskName
              << "' inside '" << targetParent << "' (Priority: " << priority
              << (branchCondition.empty() ? "" : ", Condition: " + branchCondition) << ")" << std::endl;

    Task *task = new Task(taskName, priority, branchCondition);
    if (subprocessRegistry.count(targetParent))
    {
        subprocessRegistry[targetParent]->Add(task);
    }
}

WorkflowDefinition *CustomDefinitionBuilder::getResult()
{
    subprocessRegistry.clear();
    departmentRegistry.clear();
    return WorkflowDefinitionBuilder::getResult();
}
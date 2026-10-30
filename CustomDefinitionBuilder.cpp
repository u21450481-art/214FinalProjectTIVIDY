#include "CustomDefinitionBuilder.h"
#include <stdexcept>

#include "Log.h"
#include "Subprocess.h"
#include "Task.h"
#include "Department.h"
#include "Employee.h"

void CustomDefinitionBuilder::requireBuild() const
{
    if (definition == nullptr)
        throw std::logic_error("No active blueprint: call reset(name, version) first.");
}

void CustomDefinitionBuilder::reset(const std::string &name, int version)
{
    if (name.empty())
        throw std::invalid_argument("Blueprint name cannot be empty.");

    delete definition; // discard any abandoned build
    definition = nullptr;
    subprocessRegistry.clear();
    departmentRegistry.clear();

    definition = new WorkflowDefinition(name, version);
    Subprocess *root = new Subprocess(name + "_Root");
    definition->setRoot(root);
    subprocessRegistry["ROOT"] = root;

    TLOG("[Builder] Started blueprint '" << name << "' (v" << version << ")");
}

void CustomDefinitionBuilder::addDepartment(const std::string &deptName)
{
    requireBuild();
    if (deptName.empty())
        throw std::invalid_argument("Department name cannot be empty.");
    if (departmentRegistry.count(deptName))
        throw std::invalid_argument("Department already exists: " + deptName);

    Department *dept = new Department(deptName);
    departmentRegistry[deptName] = dept;
    definition->addParticipant(dept);
    TLOG("  + [Builder] Department: " << deptName);
}

void CustomDefinitionBuilder::addEmployee(const std::string &empName, const std::string &deptName)
{
    requireBuild();
    if (empName.empty())
        throw std::invalid_argument("Employee name cannot be empty.");

    Department *dept = nullptr;
    if (!deptName.empty())
    {
        auto it = departmentRegistry.find(deptName);
        if (it == departmentRegistry.end())
            throw std::invalid_argument("Unknown department: " + deptName);
        dept = it->second;
    }

    Employee *emp = new Employee(empName, "", 0);
    if (dept)
        dept->add(emp);
    else
        definition->addParticipant(emp);
    TLOG("  + [Builder] Employee: " << empName);
}

void CustomDefinitionBuilder::addSubprocess(const std::string &subName, const std::string &parentName)
{
    requireBuild();
    if (subName.empty())
        throw std::invalid_argument("Subprocess name cannot be empty.");

    const std::string target = parentName.empty() ? "ROOT" : parentName;
    auto parent = subprocessRegistry.find(target);
    if (parent == subprocessRegistry.end())
        throw std::invalid_argument("Unknown parent subprocess: " + target);
    if (subprocessRegistry.count(subName)) // also protects "ROOT"
        throw std::invalid_argument("Subprocess already exists: " + subName);

    Subprocess *sub = new Subprocess(subName);
    parent->second->add(sub); // parent takes ownership
    subprocessRegistry[subName] = sub;
    TLOG("  + [Builder] Subprocess '" << subName << "' inside '" << target << "'");
}

void CustomDefinitionBuilder::addTask(const std::string &taskName, const std::string &parentName,
                                      int priority, const std::string &branchCondition,
                                      const std::string &deadline)
{
    requireBuild();
    if (taskName.empty())
        throw std::invalid_argument("Task name cannot be empty.");

    const std::string target = parentName.empty() ? "ROOT" : parentName;
    auto parent = subprocessRegistry.find(target);
    if (parent == subprocessRegistry.end())
        throw std::invalid_argument("Unknown parent subprocess: " + target);

    Task *task = new Task(priority, deadline, taskName, "");
    task->setBranchCondition(branchCondition);
    parent->second->add(task);
    TLOG("  + [Builder] Task '" << taskName << "' inside '" << target << "'");
}

WorkflowDefinition *CustomDefinitionBuilder::getResult()
{
    subprocessRegistry.clear(); // pointers must not outlive the build
    departmentRegistry.clear();
    return WorkflowDefinitionBuilder::getResult();
}
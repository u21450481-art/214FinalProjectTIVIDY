/**
 * @file WorkflowEngine.cpp
 * @brief Implementation of the WorkflowEngine Facade.
 */

#include "WorkflowEngine.h"
#include "WorkflowComponent.h"
#include "WorkflowIterator.h"

#include "Stage.h"

#include <sstream>

WorkflowEngine::WorkflowEngine()
    : builder(),
      director(&builder),
      blueprints(),
      instances(),
      //   mediators(),
      activityLog(),
      nextGlobalInstanceId(1)
{
    logEvent("[Engine] TIVIDY WorkflowEngine Facade initialized.");
}

WorkflowEngine::~WorkflowEngine()
{
    // std::unique_ptr maps automatically clean up all definitions, instances, and mediators
}

void WorkflowEngine::logEvent(const std::string &message)
{
    activityLog.push_back(message);
}

// blueprint Assembly (Builder Pattern)

void WorkflowEngine::assembleBlueprint(const std::string &name,
                                       int version,
                                       const std::vector<std::string> &departments,
                                       const std::vector<EmployeeSpec> &employees,
                                       const std::vector<SubprocessSpec> &subprocesses,
                                       const std::vector<TaskSpec> &tasks,
                                       bool finalizeNow)
{
    startCustomBlueprint(name, version);

    for (const std::string &dept : departments)
    {
        addCustomDepartment(dept);
    }

    for (const EmployeeSpec &emp : employees)
    {
        addCustomEmployee(emp.name, emp.department);
    }

    for (const SubprocessSpec &sub : subprocesses)
    {
        addCustomSubprocess(sub.name, sub.parent);
    }

    for (const TaskSpec &t : tasks)
    {
        addCustomTask(t.name, t.parent, t.priority, t.branchCondition);
    }

    if (finalizeNow)
    {
        finalizeCustomBlueprint();
    }
}

void WorkflowEngine::startCustomBlueprint(const std::string &name, int version)
{
    builder.reset(name, version);
    logEvent("[Builder] Started blueprint '" + name + "' (v" + std::to_string(version) + ").");
}

void WorkflowEngine::addCustomDepartment(const std::string &deptName)
{
    builder.addDepartment(deptName);
    logEvent("[Builder] + Department: " + deptName);
}

void WorkflowEngine::addCustomEmployee(const std::string &empName, const std::string &deptName)
{
    builder.addEmployee(empName, deptName);
    logEvent("[Builder] + Employee: " + empName + (deptName.empty() ? " (Standalone)" : " -> " + deptName));
}

void WorkflowEngine::addCustomSubprocess(const std::string &subName, const std::string &parent)
{
    builder.addSubprocess(subName, parent);
    logEvent("[Builder] + Subprocess: " + subName + " (under " + (parent.empty() ? "ROOT" : parent) + ")");
}

void WorkflowEngine::addCustomTask(const std::string &taskName,
                                   const std::string &parent,
                                   int priority,
                                   const std::string &condition)
{
    builder.addTask(taskName, parent, priority, "", condition);
    logEvent("[Builder] + Task: " + taskName + " (Priority " + std::to_string(priority) + ")");
}

bool WorkflowEngine::finalizeCustomBlueprint()
{
    std::unique_ptr<WorkflowDefinition> def(builder.getResult());
    if (!def)
    {
        logEvent("[Error] Cannot finalize blueprint: no active build in progress.");
        return false;
    }
    std::string defName = def->getName();
    if (blueprints.count(defName))
    {
        logEvent("[Error] Blueprint '" + defName + "' already exists.");
        return false;
    }
    blueprints[defName] = std::move(def);
    logEvent("[Builder] Finalized & registered blueprint: '" + defName + "'.");
    return true;
}

// making intrance & Lifecycle (Prototype & Mediator Patterns)

int WorkflowEngine::spawnInstance(const std::string &blueprintName)
{
    auto it = blueprints.find(blueprintName);
    if (it == blueprints.end() || it->second == nullptr)
    {
        logEvent("[Error] Blueprint '" + blueprintName + "' not found.");
        return -1;
    }

    WorkflowDefinition *def = it->second.get();

    // clone the blueprints task tree via Prototype pattern
    std::unique_ptr<WorkflowInstance> inst = def->instantiate();
    int assignedId = nextGlobalInstanceId++;

    // Re-wrap the clone with an engine-wide unique ID
    std::unique_ptr<WorkflowInstance> engineInst(
        new WorkflowInstance(assignedId,
                             def->getName(),
                             def->getVersion(),
                             inst->getRoot(),
                             nullptr));
    // Detach root from the temporary so its destructor doesn't delete the cloned tree
    inst->setRoot(nullptr);

    instances[assignedId] = std::move(engineInst);

    logEvent("[Prototype] Spawned Instance #" + std::to_string(assignedId) +
             " from '" + blueprintName + "' with isolated cloned task tree.");
    return assignedId;
}
// void WorkflowEngine::startInstance(int instanceId)
// {
//     auto medIt = mediators.find(instanceId);
//     if (medIt != mediators.end() && medIt->second != nullptr)
//     {
//         medIt->second->startWorkflow();
//         logEvent("[Mediator] Started execution on Instance #" + std::to_string(instanceId) + ".");
//         return;
//     }

//     auto instIt = instances.find(instanceId);
//     if (instIt != instances.end() && instIt->second != nullptr)
//     {
//         instIt->second->start();
//         logEvent("[Instance] Started Instance #" + std::to_string(instanceId) + ".");
//     }
// }

// void WorkflowEngine::cancelInstance(int instanceId)
// {
//     auto instIt = instances.find(instanceId);
//     if (instIt != instances.end() && instIt->second != nullptr)
//     {
//         instIt->second->stop();
//         logEvent("[Instance] Stopped/Cancelled Instance #" + std::to_string(instanceId) + ".");
//     }
// }

// TUI Data & View Getters

std::vector<std::string> WorkflowEngine::getBlueprintNames() const
{
    std::vector<std::string> names;
    for (const auto &pair : blueprints)
    {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<int> WorkflowEngine::getActiveInstanceIds() const
{
    std::vector<int> ids;
    for (const auto &pair : instances)
    {
        ids.push_back(pair.first);
    }
    return ids;
}

std::vector<std::string> WorkflowEngine::getInstanceTaskTreeStatus(int instanceId) const
{
    std::vector<std::string> lines;
    auto it = instances.find(instanceId);
    if (it == instances.end() || it->second == nullptr || it->second->getRoot() == nullptr)
    {
        lines.push_back("(No task tree available)");
        return lines;
    }

    WorkflowComponent *root = it->second->getRoot();
    std::unique_ptr<WorkflowIterator> iter(root->createIterator());

    int index = 0;
    for (iter->first(); !iter->isDone(); iter->next())
    {
        WorkflowComponent *node = iter->currentItem();
        if (node != nullptr)
        {
            std::string stateName = (node->getState() != nullptr) ? node->getState()->getName() : "Unknown";
            lines.push_back("Node #" + std::to_string(index++) + " [" + stateName + "]");
        }
    }
    return lines;
}

void WorkflowEngine::setPayloadValue(int instanceId, const std::string &key, const std::string &val)
{
    auto it = instances.find(instanceId);
    if (it != instances.end() && it->second != nullptr)
    {
        it->second->getPayload().set(key, val);
        logEvent("[Payload] Instance #" + std::to_string(instanceId) + ": Set '" + key + "' = '" + val + "'");
    }
}

std::map<std::string, std::string> WorkflowEngine::getPayloadMap(int instanceId) const
{
    auto it = instances.find(instanceId);
    if (it != instances.end() && it->second != nullptr)
    {
        return it->second->getPayload().getAll();
    }
    return {};
}

const std::vector<std::string> &WorkflowEngine::getActivityLog() const
{
    return activityLog;
}
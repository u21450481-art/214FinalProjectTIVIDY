/**
 * @file WorkflowEngine.cpp
 * @brief Implementation of the WorkflowEngine facade.
 */

#include "WorkflowEngine.h"

#include <fstream>

#include "ClaimsDataAdapter.h"
#include "ConcreteStrategies.h"
#include "DataAdapter.h"
#include "EngineMediator.h"
#include "HrDataAdapter.h"
#include "Participant.h"
#include "Stage.h"
#include "Task.h"
#include "WorkflowComponent.h"
#include "WorkflowIterator.h"

//---------- helpers ----------

AssignmentStrategy *WorkflowEngine::makeStrategy(const std::string &name)
{
    if (name == "LeastLoaded")
        return new LeastLoadedStrategy();
    if (name == "RoleBased")
        return new RoleBasedStrategy();
    if (name == "MostExperience")
        return new MostExperienceStrategy();
    return nullptr;
}

DataAdapter *WorkflowEngine::makeAdapter(const std::string &name)
{
    if (name == "claims")
        return new ClaimsDataAdapter();
    if (name == "hr")
        return new HrDataAdapter();
    return nullptr;
}

bool WorkflowEngine::stateIs(WorkflowComponent *c, const char *name)
{
    return c != nullptr && c->getState()->getName() == name;
}

// parent first, then children left to right (the same order the iterator uses, so findNode lines up)
void WorkflowEngine::collectRows(WorkflowComponent *node, int depth, std::vector<TaskRow> &out)
{
    if (node == nullptr)
        return;

    TaskRow row;
    row.depth = depth;
    row.name = node->getName();
    row.state = node->getState()->getName();
    row.isGroup = true;

    // only single tasks have an assignee, outcome and condition
    if (Task *t = dynamic_cast<Task *>(node))
    {
        row.isGroup = false;
        Participant *who = t->getAssignee();
        row.assignee = (who != nullptr) ? who->getName() : "";
        row.outcome = t->getOutcome();
        row.condition = t->getBranchCondition();
    }
    out.push_back(row);

    for (WorkflowComponent *child : node->getChildren())
        collectRows(child, depth + 1, out);
}

// walks the tree with the iterator until it reaches the row number
WorkflowComponent *WorkflowEngine::findNode(WorkflowInstance *inst, int index)
{
    if (inst == nullptr || inst->getRoot() == nullptr || index < 0)
        return nullptr;

    std::unique_ptr<WorkflowIterator> iter(inst->getRoot()->createIterator());
    int i = 0;
    for (iter->first(); !iter->isDone(); iter->next())
    {
        if (i++ == index)
            return iter->currentItem();
    }
    return nullptr;
}

//---------- setup ----------

WorkflowEngine::WorkflowEngine()
    : builder(),
      director(&builder),
      nextGlobalInstanceId(1)
{
    logEvent("[Facade] Engine started.");
}

// defined here (not in the header) because the mediator type must be complete to delete it
WorkflowEngine::~WorkflowEngine() {}

void WorkflowEngine::logEvent(const std::string &message)
{
    activityLog.push_back(message);
}

bool WorkflowEngine::fail(const std::string &message)
{
    lastError = message;
    logEvent("[Error] " + message);
    return false;
}

WorkflowInstance *WorkflowEngine::findInstance(int id) const
{
    auto it = instances.find(id);
    return (it != instances.end()) ? it->second.get() : nullptr;
}

EngineMediator *WorkflowEngine::findMediator(int id) const
{
    auto it = mediators.find(id);
    return (it != mediators.end()) ? it->second.get() : nullptr;
}

const std::vector<std::string> &WorkflowEngine::getActivityLog() const { return activityLog; }
const std::string &WorkflowEngine::getLastError() const { return lastError; }

//---------- blueprints ----------

bool WorkflowEngine::assembleBlueprint(const std::string &name,
                                       int version,
                                       const std::vector<std::string> &departments,
                                       const std::vector<EmployeeSpec> &employees,
                                       const std::vector<SubprocessSpec> &subprocesses,
                                       const std::vector<TaskSpec> &tasks)
{
    if (name.empty())
        return fail("Blueprint name cannot be empty.");
    if (blueprints.count(name))
        return fail("Blueprint '" + name + "' already exists.");
    if (tasks.empty())
        return fail("Blueprint '" + name + "' needs at least one task.");

    // the mediator treats anyone with no children as a person, so an empty department would be given tasks
    for (const std::string &dept : departments)
    {
        bool hasMember = false;
        for (const EmployeeSpec &e : employees)
            if (e.department == dept)
                hasMember = true;
        if (!hasMember)
            return fail("Department '" + dept + "' has no employees.");
    }

    try
    {
        director.construct(name, version, departments, employees, subprocesses, tasks);
    }
    catch (const std::exception &e)
    {
        return fail(std::string("Builder: ") + e.what());
    }

    blueprints[name] = std::unique_ptr<WorkflowDefinition>(builder.getResult());
    logEvent("[Builder] Created blueprint '" + name + "'.");
    return true;
}

std::vector<std::string> WorkflowEngine::getBlueprintNames() const
{
    std::vector<std::string> names;
    for (const auto &pair : blueprints)
        names.push_back(pair.first);
    return names;
}

//---------- instances ----------

int WorkflowEngine::spawnInstance(const std::string &blueprintName, const std::string &strategyName)
{
    auto bp = blueprints.find(blueprintName);
    if (bp == blueprints.end())
    {
        fail("Blueprint '" + blueprintName + "' not found.");
        return -1;
    }

    std::unique_ptr<AssignmentStrategy> strategy(makeStrategy(strategyName));
    if (!strategy)
    {
        fail("Unknown strategy '" + strategyName + "'.");
        return -1;
    }

    WorkflowDefinition *def = bp->second.get();
    int id = nextGlobalInstanceId++;

    // prototype: the instance gets its own copy of the task tree
    std::unique_ptr<WorkflowInstance> inst = def->instantiate(id);

    // the mediator owns the strategy and works out who can be assigned tasks
    std::unique_ptr<EngineMediator> med(new EngineMediator());
    med->setRoot(inst->getRoot());
    med->setCandidates(def->getParticipants());
    med->setStrategy(strategy.release());
    inst->setMediator(med.get());

    instances[id] = std::move(inst);
    mediators[id] = std::move(med);

    logEvent("[Facade] Spawned instance #" + std::to_string(id) + " from '" + blueprintName + "'.");
    return id;
}

std::vector<int> WorkflowEngine::getActiveInstanceIds() const
{
    std::vector<int> ids;
    for (const auto &pair : instances)
        ids.push_back(pair.first);
    return ids;
}

std::string WorkflowEngine::getInstanceStatus(int instanceId) const
{
    WorkflowInstance *inst = findInstance(instanceId);
    EngineMediator *med = findMediator(instanceId);
    if (!inst || !med)
        return "Unknown";
    if (med->isFinished())
        return "Finished";
    if (med->isHalted())
        return "Halted";
    return stateIs(inst->getRoot(), "NotStarted") ? "Not started" : "Running";
}

std::string WorkflowEngine::getInstanceLabel(int instanceId) const
{
    WorkflowInstance *inst = findInstance(instanceId);
    if (!inst)
        return "#" + std::to_string(instanceId) + " (missing)";
    return "#" + std::to_string(instanceId) + "  " + inst->getDefinitionName() +
           "  [" + getInstanceStatus(instanceId) + "]";
}

//---------- running ----------

bool WorkflowEngine::startInstance(int instanceId)
{
    WorkflowInstance *inst = findInstance(instanceId);
    EngineMediator *med = findMediator(instanceId);
    if (!inst || !med)
        return fail("Instance #" + std::to_string(instanceId) + " not found.");
    if (!stateIs(inst->getRoot(), "NotStarted"))
        return fail("Instance #" + std::to_string(instanceId) + " was already started.");

    med->startWorkflow();
    logEvent("[Facade] Started instance #" + std::to_string(instanceId) + ".");
    return true;
}

bool WorkflowEngine::completeTask(int instanceId, int taskIndex)
{
    EngineMediator *med = findMediator(instanceId);
    WorkflowComponent *task = findNode(findInstance(instanceId), taskIndex);
    if (!med || !task)
        return fail("Task not found.");
    if (!med->completeTask(task))
        return fail("Could not complete '" + task->getName() + "'.");

    logEvent("[Facade] Completed '" + task->getName() + "'.");
    return true;
}

bool WorkflowEngine::rejectTask(int instanceId, int taskIndex)
{
    EngineMediator *med = findMediator(instanceId);
    WorkflowComponent *task = findNode(findInstance(instanceId), taskIndex);
    if (!med || !task)
        return fail("Task not found.");
    if (!med->rejectTask(task))
        return fail("Could not reject '" + task->getName() + "'.");

    logEvent("[Facade] Rejected '" + task->getName() + "'.");
    return true;
}

bool WorkflowEngine::cancelTask(int instanceId, int taskIndex)
{
    EngineMediator *med = findMediator(instanceId);
    WorkflowComponent *task = findNode(findInstance(instanceId), taskIndex);
    if (!med || !task)
        return fail("Task not found.");
    if (!med->cancelTask(task))
        return fail("Could not cancel '" + task->getName() + "'.");

    logEvent("[Facade] Cancelled '" + task->getName() + "'.");
    return true;
}

std::vector<TaskRow> WorkflowEngine::getTaskRows(int instanceId) const
{
    std::vector<TaskRow> rows;
    WorkflowInstance *inst = findInstance(instanceId);
    if (inst)
        collectRows(inst->getRoot(), 0, rows);
    return rows;
}

//---------- strategy ----------

std::vector<std::string> WorkflowEngine::getStrategyNames() const
{
    return {"LeastLoaded", "RoleBased", "MostExperience"};
}

bool WorkflowEngine::setStrategy(int instanceId, const std::string &strategyName)
{
    EngineMediator *med = findMediator(instanceId);
    if (!med)
        return fail("Instance #" + std::to_string(instanceId) + " not found.");

    AssignmentStrategy *strategy = makeStrategy(strategyName);
    if (!strategy)
        return fail("Unknown strategy '" + strategyName + "'.");

    med->setStrategy(strategy); // the mediator owns it now
    return true;
}

std::string WorkflowEngine::getStrategyName(int instanceId) const
{
    EngineMediator *med = findMediator(instanceId);
    if (!med || med->getStrategy() == nullptr)
        return "(none)";
    return med->getStrategy()->getName();
}

//---------- adapter ----------

std::vector<std::string> WorkflowEngine::getAdapterNames() const
{
    return {"claims", "hr"};
}

bool WorkflowEngine::setAdapter(int instanceId, const std::string &adapterName)
{
    EngineMediator *med = findMediator(instanceId);
    if (!med)
        return fail("Instance #" + std::to_string(instanceId) + " not found.");

    DataAdapter *adapter = makeAdapter(adapterName);
    if (!adapter)
        return fail("Unknown adapter '" + adapterName + "'.");

    med->setDataAdapter(adapter); // the mediator owns it now
    adapterNames[instanceId] = adapterName;
    return true;
}

std::string WorkflowEngine::getAdapterName(int instanceId) const
{
    auto it = adapterNames.find(instanceId);
    return (it != adapterNames.end()) ? it->second : "none";
}

bool WorkflowEngine::importData(int instanceId, const std::string &recordId)
{
    WorkflowInstance *inst = findInstance(instanceId);
    EngineMediator *med = findMediator(instanceId);
    if (!inst || !med)
        return fail("Instance #" + std::to_string(instanceId) + " not found.");
    if (!adapterNames.count(instanceId))
        return fail("Choose an adapter first.");
    if (recordId.empty())
        return fail("Enter a record id.");

    if (!med->importData(recordId, &inst->getPayload()))
        return fail("Could not import record '" + recordId + "'.");

    logEvent("[Facade] Imported '" + recordId + "' into instance #" + std::to_string(instanceId) + ".");
    return true;
}

//---------- payload and history ----------

bool WorkflowEngine::setPayloadValue(int instanceId, const std::string &key, const std::string &value)
{
    WorkflowInstance *inst = findInstance(instanceId);
    EngineMediator *med = findMediator(instanceId);
    if (!inst || !med)
        return fail("Instance #" + std::to_string(instanceId) + " not found.");
    if (key.empty())
        return fail("Payload key cannot be empty.");

    // goes through the mediator so it can be undone
    if (!med->setData(&inst->getPayload(), key, value))
        return fail("Could not set '" + key + "'.");
    return true;
}

std::map<std::string, std::string> WorkflowEngine::getPayloadMap(int instanceId) const
{
    WorkflowInstance *inst = findInstance(instanceId);
    if (inst)
        return inst->getPayload().getAll();
    return {};
}

bool WorkflowEngine::undo(int instanceId)
{
    EngineMediator *med = findMediator(instanceId);
    if (!med || !med->undo())
        return fail("Nothing to undo.");
    return true;
}

bool WorkflowEngine::redo(int instanceId)
{
    EngineMediator *med = findMediator(instanceId);
    if (!med || !med->redo())
        return fail("Nothing to redo.");
    return true;
}

std::vector<std::string> WorkflowEngine::getHistory(int instanceId) const
{
    EngineMediator *med = findMediator(instanceId);
    if (med)
        return med->getHistory();
    return {};
}

//---------- report ----------

std::vector<std::string> WorkflowEngine::generateReport(int instanceId) const
{
    std::vector<std::string> out;
    WorkflowInstance *inst = findInstance(instanceId);
    EngineMediator *med = findMediator(instanceId);
    if (!inst || !med)
    {
        out.push_back("Instance #" + std::to_string(instanceId) + " not found.");
        return out;
    }

    std::vector<TaskRow> rows = getTaskRows(instanceId);
    int total = 0, complete = 0;
    for (const TaskRow &r : rows)
    {
        if (r.isGroup)
            continue;
        total++;
        if (r.state == "Complete")
            complete++;
    }

    out.push_back("INSTANCE #" + std::to_string(instanceId) + ": " + inst->getDefinitionName());
    out.push_back("  Status:    " + getInstanceStatus(instanceId));
    if (med->isHalted() && med->getHaltedAt() != nullptr)
        out.push_back("  Halted at: " + med->getHaltedAt()->getName());
    out.push_back("  Strategy:  " + getStrategyName(instanceId));
    out.push_back("  Adapter:   " + getAdapterName(instanceId));
    out.push_back("  Progress:  " + std::to_string(complete) + "/" + std::to_string(total) + " tasks complete");

    out.push_back("");
    out.push_back("TASKS");
    for (const TaskRow &r : rows)
    {
        std::string line = "  " + std::string(r.depth * 2, ' ') + r.name + "  [" + r.state + "]";
        if (!r.assignee.empty())
            line += "  -> " + r.assignee;
        if (!r.outcome.empty())
            line += "  outcome: " + r.outcome;
        if (!r.condition.empty())
            line += "  if " + r.condition;
        out.push_back(line);
    }

    out.push_back("");
    out.push_back("PAYLOAD");
    std::map<std::string, std::string> payload = getPayloadMap(instanceId);
    if (payload.empty())
        out.push_back("  (empty)");
    for (const auto &kv : payload)
        out.push_back("  " + kv.first + " = " + kv.second);

    std::vector<std::string> history = med->getHistory();
    out.push_back("");
    out.push_back("COMMAND HISTORY");
    if (history.empty())
        out.push_back("  (none)");
    for (const std::string &h : history)
        out.push_back("  " + h);

    return out;
}

bool WorkflowEngine::exportReport(const std::string &path)
{
    std::ofstream file(path.c_str());
    if (!file)
        return fail("Could not write to '" + path + "'.");

    for (const auto &pair : instances)
    {
        for (const std::string &line : generateReport(pair.first))
            file << line << '\n';
        file << '\n';
    }

    file << "ACTIVITY LOG\n";
    for (const std::string &line : activityLog)
        file << "  " << line << '\n';

    logEvent("[Facade] Report saved to '" + path + "'.");
    return true;
}
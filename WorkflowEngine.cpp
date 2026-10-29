/**
 * @file WorkflowEngine.cpp
 * @brief Implementation of the WorkflowEngine Facade.
 */

#include "WorkflowEngine.h"

#include <fstream>
#include <iostream>

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

// Redirects std::cout into the facade's buffer for the lifetime of the object,
// so the messages the subsystems print land in the activity log instead of
// scribbling over the TUI. Safe to nest.
class OutputCapture
{
public:
    explicit OutputCapture(WorkflowEngine &e)
        : engine(e), previous(std::cout.rdbuf(e.captured.rdbuf())) {}

    ~OutputCapture()
    {
        std::cout.rdbuf(previous);
        engine.drainCaptured();
    }

    OutputCapture(const OutputCapture &) = delete;
    OutputCapture &operator=(const OutputCapture &) = delete;

private:
    WorkflowEngine &engine;
    std::streambuf *previous;
};

namespace
{
    const std::size_t kMaxLogLines = 500;

    AssignmentStrategy *makeStrategy(const std::string &name)
    {
        if (name == "LeastLoaded")
            return new LeastLoadedStrategy();
        if (name == "RoleBased")
            return new RoleBasedStrategy();
        if (name == "MostExperience")
            return new MostExperienceStrategy();
        return nullptr;
    }

    DataAdapter *makeAdapter(const std::string &name)
    {
        if (name == "claims")
            return new ClaimsDataAdapter();
        if (name == "hr")
            return new HrDataAdapter();
        return nullptr;
    }

    bool stateIs(WorkflowComponent *c, const char *name)
    {
        return c != nullptr && c->getState() != nullptr && c->getState()->getName() == name;
    }

    // Pre-order walk (parent first, children left to right). This is the same
    // order depthFirstIterator visits nodes, so row numbers match findNode().
    void collectRows(WorkflowComponent *node, int depth, std::vector<TaskRow> &out)
    {
        if (node == nullptr)
            return;

        TaskRow row;
        row.index = static_cast<int>(out.size());
        row.depth = depth;
        row.name = node->getName();
        row.state = (node->getState() != nullptr) ? node->getState()->getName() : "Unknown";
        row.isGroup = true;

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

    // Finds the node at a row index using the Iterator pattern.
    WorkflowComponent *findNode(WorkflowInstance *inst, int index)
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

    int countPeople(Participant *p)
    {
        if (p == nullptr)
            return 0;
        std::vector<Participant *> kids = p->getChildren();
        if (kids.empty())
            return 1;
        int total = 0;
        for (Participant *k : kids)
            total += countPeople(k);
        return total;
    }

    void describePeople(Participant *p, int depth,
                        const std::map<std::string, int> &held,
                        std::vector<std::string> &out)
    {
        if (p == nullptr)
            return;

        std::string line = "  " + std::string(static_cast<std::size_t>(depth) * 2, ' ') + p->getName();
        std::vector<Participant *> kids = p->getChildren();
        if (kids.empty())
        {
            auto it = held.find(p->getName());
            line += "  (holds " + std::to_string(it == held.end() ? 0 : it->second) + " task(s))";
        }
        out.push_back(line);

        for (Participant *k : kids)
            describePeople(k, depth + 1, held, out);
    }
} // namespace

// ============================================================================
// Construction and helpers
// ============================================================================

WorkflowEngine::WorkflowEngine()
    : builder(),
      director(&builder),
      blueprints(),
      instances(),
      mediators(),
      adapterNames(),
      activityLog(),
      lastError(),
      captured(),
      nextGlobalInstanceId(1)
{
    logEvent("[Engine] TIVIDY WorkflowEngine initialized.");
}

WorkflowEngine::~WorkflowEngine() {}

void WorkflowEngine::logEvent(const std::string &message)
{
    activityLog.push_back(message);
    if (activityLog.size() > kMaxLogLines)
        activityLog.erase(activityLog.begin(),
                          activityLog.begin() + static_cast<std::ptrdiff_t>(activityLog.size() - kMaxLogLines));
}

bool WorkflowEngine::fail(const std::string &message)
{
    lastError = message;
    logEvent("[Error] " + message);
    return false;
}

void WorkflowEngine::drainCaptured()
{
    const std::string text = captured.str();
    captured.str("");
    captured.clear();

    std::istringstream lines(text);
    std::string line;
    while (std::getline(lines, line))
    {
        if (!line.empty())
            logEvent(line);
    }
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

// ============================================================================
// Blueprints
// ============================================================================

bool WorkflowEngine::assembleBlueprint(const std::string &name,
                                       int version,
                                       const std::vector<std::string> &departments,
                                       const std::vector<EmployeeSpec> &employees,
                                       const std::vector<SubprocessSpec> &subprocesses,
                                       const std::vector<TaskSpec> &tasks)
{
    OutputCapture capture(*this);
    lastError.clear();

    if (name.empty())
        return fail("Blueprint name cannot be empty.");
    if (blueprints.count(name))
        return fail("Blueprint '" + name + "' already exists.");
    if (tasks.empty())
        return fail("Blueprint '" + name + "' needs at least one task.");

    // The mediator treats a participant with no children as a person, so an
    // empty department could be handed a task. Reject it here.
    for (const std::string &dept : departments)
    {
        bool hasMember = false;
        for (const EmployeeSpec &e : employees)
        {
            if (e.department == dept)
            {
                hasMember = true;
                break;
            }
        }
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

    return registerBlueprint();
}

bool WorkflowEngine::registerBlueprint()
{
    std::unique_ptr<WorkflowDefinition> def(builder.getResult());
    if (!def)
        return fail("No finished blueprint to register.");

    const std::string defName = def->getName();
    if (blueprints.count(defName))
        return fail("Blueprint '" + defName + "' already exists.");

    blueprints[defName] = std::move(def);
    logEvent("[Builder] Registered blueprint '" + defName + "'.");
    return true;
}

std::vector<std::string> WorkflowEngine::getBlueprintNames() const
{
    std::vector<std::string> names;
    for (const auto &pair : blueprints)
        names.push_back(pair.first);
    return names;
}

// ============================================================================
// Instances
// ============================================================================

int WorkflowEngine::spawnInstance(const std::string &blueprintName, const std::string &strategyName)
{
    OutputCapture capture(*this);
    lastError.clear();

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
    const int id = nextGlobalInstanceId++;

    // Prototype: the instance owns a deep copy of the blueprint's task tree.
    std::unique_ptr<WorkflowInstance> inst = def->instantiate(id);

    // Mediator: registers itself with every node, knows who can be assigned
    // work (flattened participants) and owns the strategy.
    std::unique_ptr<EngineMediator> med(new EngineMediator());
    med->setRoot(inst->getRoot());
    med->setCandidates(def->getParticipants());
    med->setStrategy(strategy.release());
    inst->setMediator(med.get());

    instances[id] = std::move(inst);
    mediators[id] = std::move(med);

    logEvent("[Facade] Spawned instance #" + std::to_string(id) + " from '" + blueprintName +
             "' using the " + strategyName + " strategy.");
    return id;
}

bool WorkflowEngine::removeInstance(int instanceId)
{
    OutputCapture capture(*this);
    lastError.clear();

    if (!findInstance(instanceId))
        return fail("Instance #" + std::to_string(instanceId) + " not found.");

    // The mediator goes first: it points at the instance's task tree.
    mediators.erase(instanceId);
    instances.erase(instanceId);
    adapterNames.erase(instanceId);

    logEvent("[Facade] Removed instance #" + std::to_string(instanceId) + ".");
    return true;
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
           " v" + std::to_string(inst->getDefinitionVersion()) +
           "  [" + getInstanceStatus(instanceId) + "]";
}

// ============================================================================
// Execution
// ============================================================================

bool WorkflowEngine::startInstance(int instanceId)
{
    OutputCapture capture(*this);
    lastError.clear();

    WorkflowInstance *inst = findInstance(instanceId);
    EngineMediator *med = findMediator(instanceId);
    if (!inst || !med)
        return fail("Instance #" + std::to_string(instanceId) + " not found.");
    if (!stateIs(inst->getRoot(), "NotStarted"))
        return fail("Instance #" + std::to_string(instanceId) + " has already been started.");

    med->startWorkflow();
    logEvent("[Facade] Started instance #" + std::to_string(instanceId) + ".");
    return true;
}

bool WorkflowEngine::runTaskAction(int instanceId, int taskIndex, TaskAction action)
{
    OutputCapture capture(*this);
    lastError.clear();

    WorkflowInstance *inst = findInstance(instanceId);
    EngineMediator *med = findMediator(instanceId);
    if (!inst || !med)
        return fail("Instance #" + std::to_string(instanceId) + " not found.");

    WorkflowComponent *node = findNode(inst, taskIndex);
    if (!node)
        return fail("No task at row " + std::to_string(taskIndex) + ".");

    const std::string name = node->getName();
    const std::string before = node->getState()->getName();
    bool ok = false;
    const char *verb = "";

    switch (action)
    {
    case TaskAction::Complete:
        ok = med->completeTask(node);
        verb = "Complete";
        break;
    case TaskAction::Reject:
        ok = med->rejectTask(node);
        verb = "Reject";
        break;
    case TaskAction::Cancel:
        ok = med->cancelTask(node);
        verb = "Cancel";
        break;
    }

    if (!ok)
        return fail(std::string(verb) + " refused for '" + name + "' (state: " + before + ").");

    logEvent(std::string("[Facade] ") + verb + " '" + name + "' on instance #" +
             std::to_string(instanceId) + ".");
    return true;
}

bool WorkflowEngine::completeTask(int instanceId, int taskIndex)
{
    return runTaskAction(instanceId, taskIndex, TaskAction::Complete);
}

bool WorkflowEngine::rejectTask(int instanceId, int taskIndex)
{
    return runTaskAction(instanceId, taskIndex, TaskAction::Reject);
}

bool WorkflowEngine::cancelTask(int instanceId, int taskIndex)
{
    return runTaskAction(instanceId, taskIndex, TaskAction::Cancel);
}

bool WorkflowEngine::completeNextTask(int instanceId)
{
    WorkflowInstance *inst = findInstance(instanceId);
    if (!inst || !inst->getRoot())
    {
        lastError = "Instance #" + std::to_string(instanceId) + " not found.";
        return false;
    }

    // Iterator pattern: first single task (no children) that is InProgress.
    int target = -1;
    int i = 0;
    std::unique_ptr<WorkflowIterator> iter(inst->getRoot()->createIterator());
    for (iter->first(); !iter->isDone(); iter->next(), ++i)
    {
        WorkflowComponent *n = iter->currentItem();
        if (n != nullptr && n->getChildren().empty() && stateIs(n, "InProgress"))
        {
            target = i;
            break;
        }
    }

    if (target < 0)
    {
        OutputCapture capture(*this);
        return fail("No task is in progress. Start the instance first, or it may be finished.");
    }
    return runTaskAction(instanceId, target, TaskAction::Complete);
}

std::vector<TaskRow> WorkflowEngine::getTaskRows(int instanceId) const
{
    std::vector<TaskRow> rows;
    WorkflowInstance *inst = findInstance(instanceId);
    if (inst)
        collectRows(inst->getRoot(), 0, rows);
    return rows;
}

// ============================================================================
// Strategy
// ============================================================================

std::vector<std::string> WorkflowEngine::getStrategyNames() const
{
    return {"LeastLoaded", "RoleBased", "MostExperience"};
}

bool WorkflowEngine::setStrategy(int instanceId, const std::string &strategyName)
{
    OutputCapture capture(*this);
    lastError.clear();

    EngineMediator *med = findMediator(instanceId);
    if (!med)
        return fail("Instance #" + std::to_string(instanceId) + " not found.");

    std::unique_ptr<AssignmentStrategy> strategy(makeStrategy(strategyName));
    if (!strategy)
        return fail("Unknown strategy '" + strategyName + "'.");

    med->setStrategy(strategy.release()); // the mediator takes ownership
    logEvent("[Facade] Instance #" + std::to_string(instanceId) + " now uses the " +
             strategyName + " strategy.");
    return true;
}

std::string WorkflowEngine::getStrategyName(int instanceId) const
{
    EngineMediator *med = findMediator(instanceId);
    if (!med || med->getStrategy() == nullptr)
        return "(none)";
    return med->getStrategy()->getName();
}

// ============================================================================
// Adapter
// ============================================================================

std::vector<std::string> WorkflowEngine::getAdapterNames() const
{
    return {"claims", "hr"};
}

bool WorkflowEngine::setAdapter(int instanceId, const std::string &adapterName)
{
    OutputCapture capture(*this);
    lastError.clear();

    EngineMediator *med = findMediator(instanceId);
    if (!med)
        return fail("Instance #" + std::to_string(instanceId) + " not found.");

    std::unique_ptr<DataAdapter> adapter(makeAdapter(adapterName));
    if (!adapter)
        return fail("Unknown adapter '" + adapterName + "'.");

    med->setDataAdapter(adapter.release()); // the mediator takes ownership
    adapterNames[instanceId] = adapterName;
    logEvent("[Facade] Instance #" + std::to_string(instanceId) + " now uses the " +
             adapterName + " adapter.");
    return true;
}

std::string WorkflowEngine::getAdapterName(int instanceId) const
{
    auto it = adapterNames.find(instanceId);
    return (it != adapterNames.end()) ? it->second : "none";
}

bool WorkflowEngine::importData(int instanceId, const std::string &recordId)
{
    OutputCapture capture(*this);
    lastError.clear();

    WorkflowInstance *inst = findInstance(instanceId);
    EngineMediator *med = findMediator(instanceId);
    if (!inst || !med)
        return fail("Instance #" + std::to_string(instanceId) + " not found.");
    if (!adapterNames.count(instanceId))
        return fail("No adapter set for instance #" + std::to_string(instanceId) + ". Choose one first.");
    if (recordId.empty())
        return fail("Enter a record id to import.");

    if (!med->importData(recordId, &inst->getPayload()))
        return fail("Import of '" + recordId + "' failed (record not found).");

    logEvent("[Facade] Imported record '" + recordId + "' into instance #" +
             std::to_string(instanceId) + " via the " + getAdapterName(instanceId) + " adapter.");
    return true;
}

// ============================================================================
// Payload and history
// ============================================================================

bool WorkflowEngine::setPayloadValue(int instanceId, const std::string &key, const std::string &value)
{
    OutputCapture capture(*this);
    lastError.clear();

    WorkflowInstance *inst = findInstance(instanceId);
    EngineMediator *med = findMediator(instanceId);
    if (!inst || !med)
        return fail("Instance #" + std::to_string(instanceId) + " not found.");
    if (key.empty())
        return fail("Payload key cannot be empty.");

    // Through the mediator so it becomes an undoable SetDataCommand.
    if (!med->setData(&inst->getPayload(), key, value))
        return fail("Could not set '" + key + "'.");

    logEvent("[Facade] Instance #" + std::to_string(instanceId) + ": " + key + " = " + value);
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
    OutputCapture capture(*this);
    lastError.clear();

    EngineMediator *med = findMediator(instanceId);
    if (!med)
        return fail("Instance #" + std::to_string(instanceId) + " not found.");
    if (!med->undo())
        return fail("Nothing to undo on instance #" + std::to_string(instanceId) + ".");

    logEvent("[Facade] Undid the last action on instance #" + std::to_string(instanceId) + ".");
    return true;
}

bool WorkflowEngine::redo(int instanceId)
{
    OutputCapture capture(*this);
    lastError.clear();

    EngineMediator *med = findMediator(instanceId);
    if (!med)
        return fail("Instance #" + std::to_string(instanceId) + " not found.");
    if (!med->redo())
        return fail("Nothing to redo on instance #" + std::to_string(instanceId) + ".");

    logEvent("[Facade] Redid an action on instance #" + std::to_string(instanceId) + ".");
    return true;
}

std::vector<std::string> WorkflowEngine::getHistory(int instanceId) const
{
    EngineMediator *med = findMediator(instanceId);
    if (med)
        return med->getHistory();
    return {};
}

// ============================================================================
// Reports
// ============================================================================

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

    const std::vector<TaskRow> rows = getTaskRows(instanceId);
    int total = 0, complete = 0, inProgress = 0, notStarted = 0, stopped = 0;
    std::map<std::string, int> held;
    for (const TaskRow &r : rows)
    {
        if (r.isGroup)
            continue;
        ++total;
        if (r.state == "Complete")
            ++complete;
        else if (r.state == "InProgress")
            ++inProgress;
        else if (r.state == "NotStarted")
            ++notStarted;
        else
            ++stopped; // Rejected or Cancelled
        if (!r.assignee.empty())
            ++held[r.assignee];
    }

    out.push_back("INSTANCE #" + std::to_string(instanceId) + ": " + inst->getDefinitionName() +
                  " v" + std::to_string(inst->getDefinitionVersion()));
    out.push_back("  Status:    " + getInstanceStatus(instanceId));
    if (med->isHalted() && med->getHaltedAt() != nullptr)
        out.push_back("  Halted at: " + med->getHaltedAt()->getName());
    out.push_back("  Strategy:  " + getStrategyName(instanceId));
    out.push_back("  Adapter:   " + getAdapterName(instanceId));
    out.push_back("  Progress:  " + std::to_string(complete) + "/" + std::to_string(total) +
                  " tasks complete (" + std::to_string(inProgress) + " in progress, " +
                  std::to_string(notStarted) + " not started, " + std::to_string(stopped) +
                  " rejected/cancelled)");

    out.push_back("");
    out.push_back("TASKS");
    for (const TaskRow &r : rows)
    {
        std::string line = "  " + std::string(static_cast<std::size_t>(r.depth) * 2, ' ') + r.name +
                           "  [" + r.state + "]";
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
    const std::map<std::string, std::string> payload = getPayloadMap(instanceId);
    if (payload.empty())
        out.push_back("  (empty)");
    for (const auto &kv : payload)
        out.push_back("  " + kv.first + " = " + kv.second);

    const std::vector<std::string> history = med->getHistory();
    out.push_back("");
    out.push_back("COMMAND HISTORY (" + std::to_string(history.size()) + ")");
    if (history.empty())
        out.push_back("  (none)");
    for (std::size_t i = 0; i < history.size(); ++i)
        out.push_back("  " + std::to_string(i + 1) + ". " + history[i]);

    out.push_back("");
    out.push_back("PARTICIPANTS");
    auto bp = blueprints.find(inst->getDefinitionName());
    if (bp != blueprints.end())
    {
        for (Participant *p : bp->second->getParticipants())
            describePeople(p, 0, held, out);
    }
    return out;
}

std::vector<std::string> WorkflowEngine::generateSystemReport() const
{
    std::vector<std::string> out;
    out.push_back("TIVIDY SYSTEM REPORT");
    out.push_back("");
    out.push_back("BLUEPRINTS (" + std::to_string(blueprints.size()) + ")");
    for (const auto &pair : blueprints)
    {
        WorkflowDefinition *def = pair.second.get();
        std::vector<TaskRow> rows;
        collectRows(def->getRoot(), 0, rows);
        int tasks = 0;
        for (const TaskRow &r : rows)
            if (!r.isGroup)
                ++tasks;
        int people = 0;
        for (Participant *p : def->getParticipants())
            people += countPeople(p);

        out.push_back("  " + pair.first + " v" + std::to_string(def->getVersion()) + ": " +
                      std::to_string(tasks) + " tasks, " + std::to_string(people) + " people");
    }

    out.push_back("");
    out.push_back("INSTANCES (" + std::to_string(instances.size()) + ")");
    for (const auto &pair : instances)
    {
        out.push_back("  " + getInstanceLabel(pair.first) + "  strategy=" +
                      getStrategyName(pair.first) + "  adapter=" + getAdapterName(pair.first));
    }

    out.push_back("");
    std::string strategies, adapters;
    for (const std::string &s : getStrategyNames())
        strategies += (strategies.empty() ? "" : ", ") + s;
    for (const std::string &a : getAdapterNames())
        adapters += (adapters.empty() ? "" : ", ") + a;
    out.push_back("Available strategies: " + strategies);
    out.push_back("Available adapters:   " + adapters);
    out.push_back("Activity log entries: " + std::to_string(activityLog.size()));
    return out;
}

bool WorkflowEngine::exportReport(const std::string &path)
{
    lastError.clear();

    std::ofstream file(path.c_str());
    if (!file)
        return fail("Could not open '" + path + "' for writing.");

    for (const std::string &line : generateSystemReport())
        file << line << '\n';

    for (const auto &pair : instances)
    {
        file << "\n----------------------------------------\n";
        for (const std::string &line : generateReport(pair.first))
            file << line << '\n';
    }

    file << "\n----------------------------------------\nACTIVITY LOG\n";
    for (const std::string &line : activityLog)
        file << "  " << line << '\n';

    logEvent("[Facade] Report exported to '" + path + "'.");
    return true;
}
#include "EngineMediator.h"
#include "Log.h"

#include "Task.h"
#include "DataPayload.h"
#include "StartCommand.h"
#include "CompleteCommand.h"
#include "RejectCommand.h"
#include "CancelCommand.h"
#include "AssignTaskCommand.h"
#include "RecordDecisionCommand.h"
#include "SetDataCommand.h"
#include "MacroCommand.h"
#include "StandardKeys.h"

EngineMediator::EngineMediator()
    : root(nullptr), strategy(nullptr), adapter(nullptr), finished(false), halted(false), haltedAt(nullptr)
{
    for (int i = 0; i < 6; i++)
        handlers[i] = nullptr;
    handlers[static_cast<int>(WorkflowEvent::Started)] = &EngineMediator::onStarted;
    handlers[static_cast<int>(WorkflowEvent::Finished)] = &EngineMediator::onFinished;
    handlers[static_cast<int>(WorkflowEvent::Failed)] = &EngineMediator::onHalt;
    handlers[static_cast<int>(WorkflowEvent::Rejected)] = &EngineMediator::onHalt;
    handlers[static_cast<int>(WorkflowEvent::Cancelled)] = &EngineMediator::onHalt;
    handlers[static_cast<int>(WorkflowEvent::Escalated)] = &EngineMediator::onEscalated;
}

EngineMediator::~EngineMediator()
{
    delete strategy;
    delete adapter;
}

//=================Setup====================
void EngineMediator::registerTree(WorkflowComponent *c)
{
    if (!c)
        return;
    c->setMediator(this);
    std::vector<WorkflowComponent *> kids = c->getChildren();
    for (auto *k : kids)
        registerTree(k);
}

void EngineMediator::setRoot(WorkflowComponent *r)
{
    root = r;
    finished = false;
    halted = false;
    haltedAt = nullptr;
    registerTree(root);
}

void EngineMediator::setStrategy(AssignmentStrategy *s)
{
    delete strategy;
    strategy = s;
}

void EngineMediator::setDataAdapter(DataAdapter *a)
{
    delete adapter;
    adapter = a;
}

void EngineMediator::flatten(Participant *p, std::vector<Participant *> &out)
{
    if (!p)
        return;
    std::vector<Participant *> kids = p->getChildren();
    if (kids.empty())
    {
        out.push_back(p);
    }
    else
    {
        for (auto *k : kids)
            flatten(k, out);
    }
}

void EngineMediator::setCandidates(const std::vector<Participant *> &people)
{
    candidates.clear();
    for (auto *p : people)
        flatten(p, candidates);
}

void EngineMediator::setExecutionMode(WorkflowComponent *composite, ExecutionMode m)
{
    if (!composite)
        return;
    for (int i = 0; i < (int)modeKeys.size(); i++)
    {
        if (modeKeys[i] == composite)
        {
            modeValues[i] = m;
            return;
        }
    }
    modeKeys.push_back(composite);
    modeValues.push_back(m);
}

//=================Running====================
void EngineMediator::startWorkflow()
{
    if (!root)
    {
        TLOG("[Mediator] No workflow to start.");
        return;
    }
    finished = false;
    halted = false;
    haltedAt = nullptr;
    TLOG("[Mediator] Starting workflow.");
    startComponent(root);
}

void EngineMediator::notify(WorkflowComponent *sender, WorkflowEvent event)
{
    if (!sender)
        return;
    TLOG("[Mediator] Event received: " << toString(event));
    Handler h = handlers[static_cast<int>(event)];
    if (h)
        (this->*h)(sender);
}

//=================Actions (every one runs a command)====================
// runs the command through the history. Only if it worked does the mediator raise the event,
// so the mediator reacts AFTER the change is recorded (history order stays correct for undo).
bool EngineMediator::runAndNotify(Command *cmd, WorkflowComponent *sender, WorkflowEvent event)
{
    if (!history.run(cmd))
    {
        TLOG("[Mediator] Action refused, nothing changed.");
        return false;
    }
    notify(sender, event);
    return true;
}

bool EngineMediator::startTask(WorkflowComponent *c)
{
    return runAndNotify(new StartCommand(c), c, WorkflowEvent::Started);
}

bool EngineMediator::completeTask(WorkflowComponent *c)
{
    return runAndNotify(new CompleteCommand(c), c, WorkflowEvent::Finished);
}

bool EngineMediator::rejectTask(WorkflowComponent *c)
{
    return runAndNotify(new RejectCommand(c), c, WorkflowEvent::Rejected);
}

bool EngineMediator::cancelTask(WorkflowComponent *c)
{
    return runAndNotify(new CancelCommand(c), c, WorkflowEvent::Cancelled);
}

bool EngineMediator::recordDecision(Task *t, const std::string &outcome)
{
    return history.run(new RecordDecisionCommand(t, outcome));
}

bool EngineMediator::setData(DataPayload *data, const std::string &key, const std::string &value)
{
    return history.run(new SetDataCommand(data, key, value));
}

// asks the adapter for the record, then writes each standard key it filled in with a SetDataCommand.
// The commands are grouped in one MacroCommand: one history entry, one undo, all or nothing.
// Returns false (and changes nothing) if there is no adapter, no data, or the record does not exist.
bool EngineMediator::importData(const std::string &id, DataPayload *data)
{
    if (!adapter || !data)
    {
        TLOG("[Mediator] No data adapter or no data to fill; nothing imported.");
        return false;
    }

    DataPayload fetched; // the adapter fills this temporary copy first
    if (!adapter->adapt(id, fetched))
    {
        TLOG("[Mediator] Import failed: no record '" << id << "'.");
        return false;
    }

    MacroCommand *macro = new MacroCommand("Import " + id);
    for (int i = 0; i < StandardKeys::KEY_COUNT; i++)
    {
        const std::string &key = StandardKeys::ALL_KEYS[i];
        if (fetched.has(key))
            macro->add(new SetDataCommand(data, key, fetched.get(key)));
    }
    return history.run(macro);
}

// undo and redo only change objects, so the mediator's own flags are worked out again afterwards
bool EngineMediator::undo()
{
    if (!history.undo())
        return false;
    refreshStatus();
    return true;
}

bool EngineMediator::redo()
{
    if (!history.redo())
        return false;
    refreshStatus();
    return true;
}

std::vector<std::string> EngineMediator::getHistory() const
{
    return history.getHistory();
}

//=================Event handlers====================
void EngineMediator::onStarted(WorkflowComponent * /*sender*/)
{
    // Nothing to coordinate yet. A place to hook auditing or notifications later.
}

void EngineMediator::onFinished(WorkflowComponent *sender)
{
    WorkflowComponent *parent = findParent(root, sender);

    if (!parent)
    {
        if (sender == root)
        {
            finished = true;
            TLOG("[Mediator] Workflow finished.");
        }
        return; // not part of our tree: ignore
    }

    if (modeOf(parent) == ExecutionMode::Sequential && !halted)
    {
        WorkflowComponent *next = findNextReady(parent);
        if (next)
        {
            startComponent(next);
            return;
        }
    }

    // Parallel, or nothing left to start: is the whole group done?
    if (isDone(parent))
    {
        // Ask the parent's own state machine to complete (as a command); it refuses if it is not InProgress.
        // completeTask raises Finished for the parent, so a finished group may finish ITS parent.
        if (stateIs(parent, "InProgress"))
            completeTask(parent);
    }
}

void EngineMediator::onHalt(WorkflowComponent *sender)
{
    halted = true;
    haltedAt = sender;
    TLOG("[Mediator] Workflow halted. No further work will be started.");
}

void EngineMediator::onEscalated(WorkflowComponent * /*sender*/)
{
    // Escalation is resolved by the approval chain; the mediator just waits.
    TLOG("[Mediator] Work escalated. Waiting for approval chain.");
}

//=================Decisions====================
WorkflowComponent *EngineMediator::findNextReady(WorkflowComponent *parent)
{
    if (!parent)
        return nullptr;
    std::vector<WorkflowComponent *> kids = parent->getChildren();
    for (auto *k : kids)
    {
        if (isDone(k))
            continue; // earlier work is done, keep looking
        if (stateIs(k, "NotStarted"))
            return k;   // first untouched child: ready
        return nullptr; // earlier child still running or failed: blocked
    }
    return nullptr; // everything done
}

void EngineMediator::assign(WorkflowComponent *c)
{
    if (!c)
        return;
    if (!strategy)
    {
        TLOG("[Mediator] No assignment strategy set; skipping assignment.");
        return;
    }
    // the component decides: a Task returns an assign command, a group returns nullptr (no type check needed)
    Command *cmd = c->makeAssignCommand(*strategy, candidates);
    if (cmd)
        history.run(cmd);
}

void EngineMediator::startComponent(WorkflowComponent *c)
{
    std::vector<WorkflowComponent *> kids = c->getChildren();

    if (kids.empty())
    { // a single piece of work
        assign(c);
        if (startTask(c))
            c->execute();
        return;
    }

    startTask(c); // the group itself begins its lifecycle
    if (modeOf(c) == ExecutionMode::Parallel)
    {
        for (auto *k : kids)
            if (stateIs(k, "NotStarted"))
                startComponent(k);
    }
    else
    {
        WorkflowComponent *first = findNextReady(c);
        if (first)
            startComponent(first);
    }
}

//=================Helpers====================
WorkflowComponent *EngineMediator::findParent(WorkflowComponent *node,
                                              WorkflowComponent *target) const
{
    if (!node)
        return nullptr;
    std::vector<WorkflowComponent *> kids = node->getChildren();
    for (auto *k : kids)
    {
        if (k == target)
            return node;
        WorkflowComponent *p = findParent(k, target);
        if (p)
            return p;
    }
    return nullptr;
}

bool EngineMediator::isDone(WorkflowComponent *c) const
{
    std::vector<WorkflowComponent *> kids = c->getChildren();
    if (kids.empty())
        return stateIs(c, "Complete");
    for (auto *k : kids)
        if (!isDone(k))
            return false;
    return true;
}

ExecutionMode EngineMediator::modeOf(const WorkflowComponent *c) const
{
    for (int i = 0; i < (int)modeKeys.size(); i++)
    {
        if (modeKeys[i] == c)
            return modeValues[i];
    }
    return ExecutionMode::Sequential;
}

// Stage has no "isComplete()" query yet, so we compare state names, as
// Subprocess::allChildrenComplete already does. Replace if Stage gains one.
bool EngineMediator::stateIs(WorkflowComponent *c, const char *name)
{
    return c->getState()->getName() == name;
}

// finds the first component that is Rejected or Cancelled, or null if there is none
WorkflowComponent *EngineMediator::findHalted(WorkflowComponent *node) const
{
    if (!node)
        return nullptr;
    if (stateIs(node, "Rejected") || stateIs(node, "Cancelled"))
        return node;
    std::vector<WorkflowComponent *> kids = node->getChildren();
    for (auto *k : kids)
    {
        WorkflowComponent *h = findHalted(k);
        if (h)
            return h;
    }
    return nullptr;
}

// after an undo or redo, work out finished/halted again from the real states
void EngineMediator::refreshStatus()
{
    haltedAt = findHalted(root);
    halted = (haltedAt != nullptr);
    finished = (root != nullptr && stateIs(root, "Complete"));
}

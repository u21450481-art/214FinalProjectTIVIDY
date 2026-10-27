#include "EngineMediator.h"

#include <iostream>

EngineMediator::EngineMediator()
    : root(nullptr), finished(false), halted(false), haltedAt(nullptr) {
    handlers[WorkflowEvent::Started]   = &EngineMediator::onStarted;
    handlers[WorkflowEvent::Finished]  = &EngineMediator::onFinished;
    handlers[WorkflowEvent::Failed]    = &EngineMediator::onHalt;
    handlers[WorkflowEvent::Rejected]  = &EngineMediator::onHalt;
    handlers[WorkflowEvent::Cancelled] = &EngineMediator::onHalt;
    handlers[WorkflowEvent::Escalated] = &EngineMediator::onEscalated;
}

EngineMediator::~EngineMediator() {}

//=================Setup====================
void EngineMediator::registerTree(WorkflowComponent* c) {
    if (!c) return;
    c->setMediator(this);
    std::vector<WorkflowComponent*> kids = c->getChildren();
    for (auto* k : kids) registerTree(k);
}

void EngineMediator::setRoot(WorkflowComponent* r) {
    root = r;
    finished = false;
    halted = false;
    haltedAt = nullptr;
    registerTree(root);
}

void EngineMediator::setStrategy(std::unique_ptr<AssignmentStrategy> s) {
    strategy = std::move(s);
}

void EngineMediator::flatten(Participant* p, std::vector<Participant*>& out) {
    if (!p) return;
    std::vector<Participant*> kids = p->getChildren();
    if (kids.empty()) {
        out.push_back(p);
    } else {
        for (auto* k : kids) flatten(k, out);
    }
}

void EngineMediator::setCandidates(const std::vector<Participant*>& people) {
    candidates.clear();
    for (auto* p : people) flatten(p, candidates);
}

void EngineMediator::setExecutionMode(WorkflowComponent* composite, ExecutionMode m) {
    if (composite) modes[composite] = m;
}

//=================Running====================
void EngineMediator::startWorkflow() {
    if (!root) {
        std::cout << "[Mediator] No workflow to start.\n";
        return;
    }
    finished = false;
    halted = false;
    haltedAt = nullptr;
    std::cout << "[Mediator] Starting workflow.\n";
    startComponent(root);
}

void EngineMediator::notify(WorkflowComponent* sender, WorkflowEvent event) {
    if (!sender) return;
    std::cout << "[Mediator] Event received: " << toString(event) << "\n";
    auto it = handlers.find(event);
    if (it != handlers.end()) (this->*(it->second))(sender);
}

//=================Event handlers====================
void EngineMediator::onStarted(WorkflowComponent* /*sender*/) {
    // Nothing to coordinate yet. A place to hook auditing or notifications later.
}

void EngineMediator::onFinished(WorkflowComponent* sender) {
    WorkflowComponent* parent = findParent(root, sender);

    if (!parent) {
        if (sender == root) {
            finished = true;
            std::cout << "[Mediator] Workflow finished.\n";
        }
        return; // not part of our tree: ignore
    }

    if (modeOf(parent) == ExecutionMode::Sequential) {
        WorkflowComponent* next = findNextReady(parent);
        if (next) {
            startComponent(next);
            return;
        }
    }

    // Parallel, or nothing left to start: is the whole group done?
    if (isDone(parent)) {
        // Ask the parent's own state machine to complete; it refuses if it is not InProgress.
        if (stateIs(parent, "InProgress")) parent->complete();
        onFinished(parent); // a finished group may finish ITS parent
    }
}

void EngineMediator::onHalt(WorkflowComponent* sender) {
    halted = true;
    haltedAt = sender;
    std::cout << "[Mediator] Workflow halted. No further work will be started.\n";
}

void EngineMediator::onEscalated(WorkflowComponent* /*sender*/) {
    // Escalation is resolved by the approval chain; the mediator just waits.
    std::cout << "[Mediator] Work escalated. Waiting for approval chain.\n";
}

//=================Decisions====================
WorkflowComponent* EngineMediator::findNextReady(WorkflowComponent* parent) {
    if (!parent) return nullptr;
    std::vector<WorkflowComponent*> kids = parent->getChildren();
    for (auto* k : kids) {
        if (isDone(k)) continue;                  // earlier work is done, keep looking
        if (stateIs(k, "NotStarted")) return k;   // first untouched child: ready
        return nullptr;                           // earlier child still running or failed: blocked
    }
    return nullptr;                               // everything done
}

void EngineMediator::assign(WorkflowComponent* c) {
    if (!c) return;
    if (!strategy) {
        std::cout << "[Mediator] No assignment strategy set; skipping assignment.\n";
        return;
    }
    std::cout << "[Mediator] Assigning using " << strategy->getName() << " strategy.\n";
    c->requestAssignment(*strategy, candidates);
}

void EngineMediator::startComponent(WorkflowComponent* c) {
    std::vector<WorkflowComponent*> kids = c->getChildren();

    if (kids.empty()) { // a single piece of work
        assign(c);
        c->execute();
        return;
    }

    c->start(); // the group itself begins its lifecycle
    if (modeOf(c) == ExecutionMode::Parallel) {
        for (auto* k : kids)
            if (stateIs(k, "NotStarted")) startComponent(k);
    } else {
        WorkflowComponent* first = findNextReady(c);
        if (first) startComponent(first);
    }
}

//=================Helpers====================
WorkflowComponent* EngineMediator::findParent(WorkflowComponent* node,
                                              WorkflowComponent* target) const {
    if (!node) return nullptr;
    std::vector<WorkflowComponent*> kids = node->getChildren();
    for (auto* k : kids) {
        if (k == target) return node;
        WorkflowComponent* p = findParent(k, target);
        if (p) return p;
    }
    return nullptr;
}

bool EngineMediator::isDone(WorkflowComponent* c) const {
    std::vector<WorkflowComponent*> kids = c->getChildren();
    if (kids.empty()) return stateIs(c, "Complete");
    for (auto* k : kids)
        if (!isDone(k)) return false;
    return true;
}

ExecutionMode EngineMediator::modeOf(const WorkflowComponent* c) const {
    auto it = modes.find(c);
    return it == modes.end() ? ExecutionMode::Sequential : it->second;
}

// Stage has no "isComplete()" query yet, so we compare state names, as
// Subprocess::allChildrenComplete already does. Replace if Stage gains one.
bool EngineMediator::stateIs(WorkflowComponent* c, const char* name) {
    return c->getState()->getName() == name;
}

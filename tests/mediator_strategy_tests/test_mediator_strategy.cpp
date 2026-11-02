// Unit tests for the Mediator (EngineMediator) and the Strategy classes.
// Strategy tests use stub people, so they test the rules alone.
// Mediator tests use a recording strategy, so they test the mediator alone.
// Run with:  make unit
#include <iostream>
#include <string>
#include <vector>

#include "ConcreteStages.h"
#include "ConcreteStrategies.h"
#include "DataPayload.h"
#include "Department.h"
#include "EngineMediator.h"
#include "Subprocess.h"
#include "Task.h"

static int total = 0, failed = 0;
#define CHECK(msg, cond) do { ++total; if (cond) std::cerr << "  PASS  " << (msg) << "\n"; \
    else { ++failed; std::cerr << "  FAIL  " << (msg) << "\n"; } } while (0)
#define SECTION(t) std::cerr << "\n== " << t << " ==\n"

//---------------------------------------------------------------- helpers
// A person whose role, workload and experience the test controls directly.
struct Stub : Participant {
    std::string nm, rl; int load, exp;
    Stub(const std::string& n, const std::string& r = "", int w = 0, int e = 0) : nm(n), rl(r), load(w), exp(e) {}
    void Operation() override {}
    std::string getName() override { return nm; }
    std::string getRole() const override { return rl; }
    int getWorkload() const override { return load; }
    int getExperience() const override { return exp; }
};

// A strategy that records how the mediator uses it, and counts live copies (to check ownership).
struct Recording : AssignmentStrategy {
    static int alive;
    Participant* answer;
    int calls;
    std::vector<const Task*> tasks;
    std::vector<Participant*> lastCandidates;
    explicit Recording(Participant* a = nullptr) : answer(a), calls(0) { ++alive; }
    ~Recording() { --alive; }
    Participant* assign(const Task& t, const std::vector<Participant*>& c) override {
        ++calls; tasks.push_back(&t); lastCandidates = c; return answer;
    }
    std::string getName() const override { return "Recording"; }
};
int Recording::alive = 0;

static Task* T(const std::string& name, const std::string& role = "") { return new Task(1, "d", name, role); }
static std::string st(WorkflowComponent* c) { return c->getState()->getName(); }
static Task strategyTask(const std::string& role = "") { return Task(1, "d", "t", role); }

//================================================================ STRATEGY
static void testRoleBased() {
    SECTION("RoleBasedStrategy");
    Stub a("A", "HR", 3), b("B", "HR", 1), c("C", "IT", 0);
    Task needHR = strategyTask("HR"), needIT = strategyTask("IT"), needLegal = strategyTask("Legal"), needAny = strategyTask();
    {
        RoleBasedStrategy s;
        std::vector<Participant*> all = { &a, &b, &c };
        CHECK("only people with the role are considered", s.assign(needIT, all) == &c);
        CHECK("lowest workload wins among the matching people", s.assign(needHR, all) == &b);
        CHECK("nobody has the role: returns nullptr", s.assign(needLegal, all) == nullptr);
    }
    {
        RoleBasedStrategy s; // fresh rotation
        std::vector<Participant*> all = { &c, &a, &b };
        CHECK("a task with no required role accepts everyone (lowest workload)", s.assign(needAny, all) == &c);
    }
    {
        RoleBasedStrategy s;
        std::vector<Participant*> none;
        CHECK("empty candidate list: nullptr, no crash", s.assign(needHR, none) == nullptr);
        std::vector<Participant*> holes = { nullptr, &a, nullptr };
        CHECK("null entries are skipped", s.assign(needHR, holes) == &a);
    }
    {
        Stub x("X", "HR"), y("Y", "HR"), z("Z", "HR");
        RoleBasedStrategy s;
        std::vector<Participant*> same = { &x, &y, &z };
        Participant* r1 = s.assign(needHR, same); Participant* r2 = s.assign(needHR, same);
        Participant* r3 = s.assign(needHR, same); Participant* r4 = s.assign(needHR, same);
        CHECK("equal candidates take turns instead of always the first", r1 == &x && r2 == &y && r3 == &z && r4 == &x);
    }
    CHECK("name", RoleBasedStrategy().getName() == "RoleBased");
}

static void testLeastLoaded() {
    SECTION("LeastLoadedStrategy");
    Task t = strategyTask();
    {
        Stub a("A", "", 4), b("B", "", 1), c("C", "", 2);
        LeastLoadedStrategy s;
        std::vector<Participant*> all = { &a, &b, &c };
        CHECK("fewest active tasks wins", s.assign(t, all) == &b);
        b.load = 9;
        CHECK("reacts to workload changes between calls", s.assign(t, all) == &c);
    }
    {
        Stub x("X"), y("Y"), z("Z");
        LeastLoadedStrategy s;
        std::vector<Participant*> same = { &x, &y, &z };
        CHECK("idle people take turns: X, Y, Z, X",
              s.assign(t, same) == &x && s.assign(t, same) == &y && s.assign(t, same) == &z && s.assign(t, same) == &x);
    }
    {
        Stub x("X", "", 2), y("Y", "", 0);
        LeastLoadedStrategy s;
        std::vector<Participant*> two = { &x, &y };
        CHECK("rotation never beats a lower workload", s.assign(t, two) == &y && s.assign(t, two) == &y);
    }
    {
        LeastLoadedStrategy s;
        std::vector<Participant*> none, holes = { nullptr, nullptr };
        CHECK("empty list: nullptr", s.assign(t, none) == nullptr);
        CHECK("only null entries: nullptr", s.assign(t, holes) == nullptr);
    }
    {
        Stub a("A"), b("B"), c("C");
        LeastLoadedStrategy s;
        std::vector<Participant*> three = { &a, &b, &c };
        s.assign(t, three); s.assign(t, three); s.assign(t, three); // rotation now points past the end
        std::vector<Participant*> shrunk = { &a, &b };
        Participant* p = s.assign(t, shrunk);
        CHECK("candidate list shrinking between calls is safe", p == &a || p == &b);
    }
    CHECK("name", LeastLoadedStrategy().getName() == "LeastLoaded");
}

static void testMostExperience() {
    SECTION("MostExperienceStrategy");
    Task t = strategyTask();
    {
        Stub a("A", "", 0, 2), b("B", "", 0, 9), c("C", "", 0, 5);
        MostExperienceStrategy s;
        std::vector<Participant*> all = { &a, &b, &c };
        CHECK("most experienced wins", s.assign(t, all) == &b);
    }
    {
        Stub a("A", "", 3, 9), b("B", "", 1, 9);
        MostExperienceStrategy s;
        std::vector<Participant*> two = { &a, &b };
        CHECK("equal experience: lower workload wins", s.assign(t, two) == &b);
    }
    {
        Stub a("A", "", 0, 7), b("B", "", 0, 7);
        MostExperienceStrategy s;
        std::vector<Participant*> two = { &a, &b };
        CHECK("equal experience and workload: take turns", s.assign(t, two) == &a && s.assign(t, two) == &b);
    }
    {
        Stub a("A", "", 0, 0), b("B", "", 0, 0);
        MostExperienceStrategy s;
        std::vector<Participant*> none;
        CHECK("zero experience is still a valid pick", s.assign(t, std::vector<Participant*>{ &a, &b }) != nullptr);
        CHECK("empty list: nullptr", s.assign(t, none) == nullptr);
    }
    CHECK("name", MostExperienceStrategy().getName() == "MostExperience");
}

static void testStrategyPolymorphism() {
    SECTION("Strategies behind the common interface");
    Stub junior("Junior", "", 0, 1), senior("Senior", "", 5, 20);
    std::vector<Participant*> pool = { &junior, &senior };
    Task t = strategyTask();
    AssignmentStrategy* rules[3] = { new RoleBasedStrategy(), new LeastLoadedStrategy(), new MostExperienceStrategy() };
    CHECK("same call, different answers: LeastLoaded picks Junior", rules[1]->assign(t, pool) == &junior);
    CHECK("same call, different answers: MostExperience picks Senior", rules[2]->assign(t, pool) == &senior);
    CHECK("every strategy has a distinct name", rules[0]->getName() != rules[1]->getName() && rules[1]->getName() != rules[2]->getName());
    for (int i = 0; i < 3; i++) delete rules[i]; // virtual destructor: no leak under the sanitizer
}

//================================================================ MEDIATOR
static void testMediatorSetup() {
    SECTION("Mediator: setup and safety");
    {
        EngineMediator m;
        m.startWorkflow();
        CHECK("no root: startWorkflow does nothing", !m.isFinished() && !m.isHalted());
        m.notify(nullptr, WorkflowEvent::Finished);
        CHECK("notify(nullptr) is ignored", !m.isFinished());
        CHECK("no strategy by default", m.getStrategy() == nullptr);
        CHECK("undo on an empty history returns false", !m.undo());
        CHECK("redo on an empty history returns false", !m.redo());
    }
    {
        int before = Recording::alive;
        {
            EngineMediator m;
            m.setStrategy(new Recording());
            CHECK("the mediator holds the strategy it was given", m.getStrategy() != nullptr && Recording::alive == before + 1);
            m.setStrategy(new Recording());
            CHECK("replacing the strategy deletes the old one", Recording::alive == before + 1);
            m.setStrategy(nullptr);
            CHECK("setStrategy(nullptr) clears it and deletes the old one", m.getStrategy() == nullptr && Recording::alive == before);
            m.setStrategy(new Recording());
        }
        CHECK("the mediator deletes its strategy when it is destroyed", Recording::alive == before);
    }
    {
        Subprocess* root = new Subprocess();
        Task* a = T("A"); root->add(a);
        EngineMediator m;
        m.setRoot(root);
        Subprocess* other = new Subprocess();
        Task* late = T("Late"); other->add(late);
        m.notify(late, WorkflowEvent::Finished);
        CHECK("an event from a component outside the tree is ignored", !m.isFinished());
        delete other; delete root;
    }
}

static void testMediatorFlow() {
    SECTION("Mediator: sequential and parallel flow");
    {
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *b = T("B"), *c = T("C");
        root->add(a); root->add(b); root->add(c);
        EngineMediator m; m.setRoot(root);
        m.startWorkflow();
        CHECK("only the first task starts", st(a) == "InProgress" && st(b) == "NotStarted" && st(c) == "NotStarted");
        CHECK("completeTask reports success", m.completeTask(a));
        CHECK("completing A starts B only", st(a) == "Complete" && st(b) == "InProgress" && st(c) == "NotStarted");
        m.completeTask(b);
        CHECK("completing B starts C", st(c) == "InProgress" && !m.isFinished());
        m.completeTask(c);
        CHECK("completing the last task finishes the workflow", m.isFinished() && st(root) == "Complete");
        delete root;
    }
    {
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *b = T("B");
        root->add(a); root->add(b);
        EngineMediator m; m.setRoot(root); m.startWorkflow();
        CHECK("completing a task that never started is refused", !m.completeTask(b));
        CHECK("a refused action changes nothing", st(b) == "NotStarted" && st(a) == "InProgress");
        size_t before = m.getHistory().size();
        m.completeTask(b);
        CHECK("a refused action adds nothing to the history", m.getHistory().size() == before);
        m.completeTask(a);
        CHECK("completing the same task twice is refused the second time", !m.completeTask(a));
        delete root;
    }
    {
        Subprocess *root = new Subprocess(), *par = new Subprocess();
        Task *first = T("First"), *p1 = T("P1"), *p2 = T("P2"), *p3 = T("P3"), *last = T("Last");
        par->add(p1); par->add(p2); par->add(p3);
        root->add(first); root->add(par); root->add(last);
        EngineMediator m; m.setRoot(root);
        m.setExecutionMode(par, ExecutionMode::Parallel);
        m.startWorkflow(); m.completeTask(first);
        CHECK("a parallel group starts all its children together", st(p1) == "InProgress" && st(p2) == "InProgress" && st(p3) == "InProgress");
        CHECK("the task after the group waits", st(last) == "NotStarted");
        m.completeTask(p2); m.completeTask(p3);
        CHECK("two of three done: the group is not finished", st(last) == "NotStarted");
        m.completeTask(p1);
        CHECK("last child done: the group completes and the next task starts", st(par) == "Complete" && st(last) == "InProgress");
        m.completeTask(last);
        CHECK("the workflow finishes", m.isFinished());
        delete root;
    }
    {
        Subprocess *root = new Subprocess(), *inner = new Subprocess(), *innermost = new Subprocess();
        Task* deep = T("Deep");
        innermost->add(deep); inner->add(innermost); root->add(inner);
        EngineMediator m; m.setRoot(root); m.startWorkflow();
        m.completeTask(deep);
        CHECK("finishing a deeply nested task finishes every group above it", st(innermost) == "Complete" && st(inner) == "Complete" && m.isFinished());
        delete root;
    }
    {
        Task* solo = T("Solo");
        EngineMediator m; m.setRoot(solo); m.startWorkflow();
        CHECK("a single task can be the whole workflow", st(solo) == "InProgress");
        m.completeTask(solo);
        CHECK("and finishing it finishes the workflow", m.isFinished());
        delete solo;
    }
}

static void testMediatorFailures() {
    SECTION("Mediator: rejection, cancellation, escalation");
    for (int mode = 0; mode < 2; mode++) {
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *b = T("B"), *c = T("C");
        root->add(a); root->add(b); root->add(c);
        EngineMediator m; m.setRoot(root); m.startWorkflow(); m.completeTask(a);
        bool ok = mode == 0 ? m.rejectTask(b) : m.cancelTask(b);
        CHECK(mode == 0 ? "rejecting a running task works" : "cancelling a running task works", ok);
        CHECK(mode == 0 ? "a rejection halts the workflow" : "a cancellation halts the workflow", m.isHalted() && m.getHaltedAt() == b);
        CHECK("nothing new starts after a halt", st(c) == "NotStarted" && !m.isFinished());
        delete root;
    }
    {
        Subprocess *root = new Subprocess(), *par = new Subprocess();
        Task *p1 = T("P1"), *p2 = T("P2"), *after = T("After");
        par->add(p1); par->add(p2); root->add(par); root->add(after);
        EngineMediator m; m.setRoot(root); m.setExecutionMode(par, ExecutionMode::Parallel); m.startWorkflow();
        m.rejectTask(p1);
        m.completeTask(p2);
        CHECK("a sibling can still finish after a halt, but the group stays unfinished and later work waits",
              st(p2) == "Complete" && st(par) != "Complete" && st(after) == "NotStarted");
        delete root;
    }
    {
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *b = T("B");
        root->add(a); root->add(b);
        EngineMediator m; m.setRoot(root); m.startWorkflow();
        m.notify(a, WorkflowEvent::Escalated);
        CHECK("an escalation neither halts nor advances", !m.isHalted() && st(b) == "NotStarted" && st(a) == "InProgress");
        m.notify(a, WorkflowEvent::Started);
        CHECK("a Started event changes nothing", st(b) == "NotStarted");
        delete root;
    }
    {
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *b = T("B");
        root->add(a); root->add(b);
        EngineMediator m; m.setRoot(root); m.startWorkflow();
        m.completeTask(a);
        m.notify(a, WorkflowEvent::Finished); // a duplicate event for the same task
        CHECK("a duplicate Finished event does not start anything twice", st(b) == "InProgress" && !m.isFinished());
        delete root;
    }
}

static void testFindNextReady() {
    SECTION("Mediator: findNextReady and branch conditions");
    {
        EngineMediator m;
        CHECK("findNextReady(nullptr) is nullptr", m.findNextReady(nullptr) == nullptr);
    }
    {
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *b = T("B"), *c = T("C");
        root->add(a); root->add(b); root->add(c);
        EngineMediator m; m.setRoot(root);
        CHECK("nothing started: the first task is next", m.findNextReady(root) == a);
        m.startTask(a);
        CHECK("an earlier task still running blocks the rest", m.findNextReady(root) == nullptr);
        m.completeTask(a);
        CHECK("completing A started B, so nothing else is ready yet", st(b) == "InProgress" && m.findNextReady(root) == nullptr);
        delete root;
    }
    {
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *gold = T("Gold only"), *c = T("C");
        gold->setBranchCondition("tier=gold");
        root->add(a); root->add(gold); root->add(c);
        DataPayload data; data.set("tier", "silver");
        EngineMediator m; m.setRoot(root); m.setPayload(&data); m.startWorkflow();
        m.completeTask(a);
        CHECK("a task whose condition does not hold is skipped", st(gold) == "NotStarted" && st(c) == "InProgress");
        m.completeTask(c);
        CHECK("and the workflow still finishes without it", m.isFinished());
        delete root;
    }
    {
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *gold = T("Gold only");
        gold->setBranchCondition("tier=gold");
        root->add(a); root->add(gold);
        DataPayload data; data.set("tier", "gold");
        EngineMediator m; m.setRoot(root); m.setPayload(&data); m.startWorkflow();
        m.completeTask(a);
        CHECK("a task whose condition holds does run", st(gold) == "InProgress");
        delete root;
    }
}

static void testMediatorWithStrategy() {
    SECTION("Mediator + Strategy contract");
    {
        Subprocess *root = new Subprocess(), *group = new Subprocess();
        Task *a = T("A", "HR"), *b = T("B"), *c = T("C");
        group->add(b); group->add(c);
        root->add(a); root->add(group);
        Stub person("P");
        Recording* rec = new Recording(&person);
        EngineMediator m; m.setRoot(root); m.setStrategy(rec); m.setCandidates({ &person });
        m.startWorkflow();
        CHECK("the strategy is asked once for the first task, not for groups", rec->calls == 1 && rec->tasks[0] == a);
        CHECK("the answer is applied to the task", a->getAssignee() == &person);
        m.completeTask(a);
        CHECK("it is asked again for the next task", rec->calls == 2 && rec->tasks[1] == b);
        m.completeTask(b);
        CHECK("and once more for the last", rec->calls == 3 && rec->tasks[2] == c);
        m.completeTask(c);
        CHECK("finishing tasks never asks the strategy again", rec->calls == 3 && m.isFinished());
        delete root;
    }
    {
        Subprocess* root = new Subprocess();
        Task* a = T("A"); root->add(a);
        Recording* rec = new Recording(nullptr);
        EngineMediator m; m.setRoot(root); m.setStrategy(rec); m.startWorkflow();
        CHECK("a strategy that finds nobody does not stop the task starting", st(a) == "InProgress" && a->getAssignee() == nullptr);
        delete root;
    }
    {
        Subprocess* root = new Subprocess();
        Task* a = T("A"); root->add(a);
        EngineMediator m; m.setRoot(root); m.startWorkflow();
        CHECK("no strategy at all: the task still starts, unassigned", st(a) == "InProgress" && a->getAssignee() == nullptr);
        delete root;
    }
    {
        Department* dept = new Department("Support");
        Stub* x = new Stub("X"); Stub* y = new Stub("Y");
        Department* inner = new Department("Inner");
        Stub* z = new Stub("Z");
        inner->add(z); dept->add(x); dept->add(y); dept->add(inner);
        Subprocess* root = new Subprocess(); Task* a = T("A"); root->add(a);
        Recording* rec = new Recording(x);
        EngineMediator m; m.setRoot(root); m.setStrategy(rec);
        std::vector<Participant*> pool; pool.push_back(dept);
        m.setCandidates(pool);
        m.startWorkflow();
        CHECK("departments are flattened: the strategy sees only individuals", rec->lastCandidates.size() == 3);
        bool noDept = true;
        for (size_t i = 0; i < rec->lastCandidates.size(); i++) if (rec->lastCandidates[i] == dept || rec->lastCandidates[i] == inner) noDept = false;
        CHECK("including people inside nested departments", noDept);
        delete root; delete dept;
    }
    {
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *b = T("B");
        root->add(a); root->add(b);
        Stub first("First"), second("Second");
        Recording* r1 = new Recording(&first);
        EngineMediator m; m.setRoot(root); m.setStrategy(r1); m.setCandidates({ &first, &second });
        m.startWorkflow();
        m.setStrategy(new Recording(&second)); // swap while running
        m.completeTask(a);
        CHECK("a strategy swapped mid-run is used for the next task", a->getAssignee() == &first && b->getAssignee() == &second);
        delete root;
    }
}

static void testUndoRedo() {
    SECTION("Mediator: undo and redo");
    {
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *b = T("B");
        root->add(a); root->add(b);
        EngineMediator m; m.setRoot(root); m.startWorkflow();
        m.rejectTask(a);
        CHECK("reject halts", m.isHalted());
        CHECK("undo reports success", m.undo());
        CHECK("undoing a rejection clears the halt and restores the task", !m.isHalted() && st(a) == "InProgress");
        CHECK("redo reports success", m.redo());
        CHECK("redoing the rejection halts again", m.isHalted() && st(a) == "Rejected");
        m.undo();
        m.completeTask(a);
        CHECK("a new action after an undo continues the flow", st(b) == "InProgress");
        delete root;
    }
    {
        Subprocess* root = new Subprocess();
        Task* a = T("A"); root->add(a);
        EngineMediator m; m.setRoot(root); m.startWorkflow();
        size_t beforeRun = m.getHistory().size();
        m.completeTask(a);
        CHECK("each action is recorded in the history", m.getHistory().size() > beforeRun);
        CHECK("the workflow is finished before undoing", m.isFinished());
        while (m.undo()) {}
        CHECK("undoing everything puts the task back to NotStarted", st(a) == "NotStarted" && !m.isFinished());
        CHECK("nothing left to undo", !m.undo());
        while (m.redo()) {}
        CHECK("redoing everything finishes the workflow again", m.isFinished());
        delete root;
    }
}


int main() {
    std::streambuf* chatter = std::cout.rdbuf(nullptr); // silence the engine's own output

    testRoleBased();
    testLeastLoaded();
    testMostExperience();
    testStrategyPolymorphism();
    testMediatorSetup();
    testMediatorFlow();
    testMediatorFailures();
    testFindNextReady();
    testMediatorWithStrategy();
    testUndoRedo();

    std::cout.rdbuf(chatter);
    std::cerr << "\n" << (total - failed) << "/" << total << " checks passed\n";
    return failed ? 1 : 0;
}

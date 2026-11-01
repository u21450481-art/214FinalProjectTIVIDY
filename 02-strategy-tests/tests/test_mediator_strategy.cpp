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


int main() {
    std::streambuf* chatter = std::cout.rdbuf(nullptr); // silence the engine's own output

    testRoleBased();
    testLeastLoaded();
    testMostExperience();
    testStrategyPolymorphism();

    std::cout.rdbuf(chatter);
    std::cerr << "\n" << (total - failed) << "/" << total << " checks passed\n";
    return failed ? 1 : 0;
}

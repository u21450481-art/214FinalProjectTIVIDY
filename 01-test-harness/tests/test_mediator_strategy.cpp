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


int main() {
    std::streambuf* chatter = std::cout.rdbuf(nullptr); // silence the engine's own output


    std::cout.rdbuf(chatter);
    std::cerr << "\n" << (total - failed) << "/" << total << " checks passed\n";
    return failed ? 1 : 0;
}

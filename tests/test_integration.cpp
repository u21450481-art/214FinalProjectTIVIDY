// Integration tests: EngineMediator + Strategy driving the real composites, plus a facade smoke test.
// Run with:  make test
#include <iostream>
#include <string>
#include <vector>

#include "ConcreteStages.h"
#include "ConcreteStrategies.h"
#include "Department.h"
#include "Employee.h"
#include "EngineMediator.h"
#include "Subprocess.h"
#include "Task.h"
#include "WorkflowEngine.h"

static int total = 0, failed = 0;
#define CHECK(msg, cond) do { ++total; if (cond) std::cerr << "  PASS  " << msg << "\n"; \
    else { ++failed; std::cerr << "  FAIL  " << msg << "\n"; } } while (0)
#define SECTION(t) std::cerr << "\n== " << t << " ==\n"

static Task* T(const std::string& name, const std::string& role = "") { return new Task(1, "d", name, role); }
static std::string st(WorkflowComponent* c) { return c->getState()->getName(); }
static std::string who(Task* t) { return t->getAssignee() ? t->getAssignee()->getName() : "nobody"; }

int main() {
    std::streambuf* engineChatter = std::cout.rdbuf(nullptr); // silence the engine's own output

    SECTION("1. Sequential flow");
    {
        Employee pat("Pat", "any", 1);
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *b = T("B"), *c = T("C");
        root->add(a); root->add(b); root->add(c);
        EngineMediator m; m.setRoot(root); m.setStrategy(new LeastLoadedStrategy()); m.setCandidates({ &pat });
        m.startWorkflow();
        CHECK("root and first task start, the rest wait", st(root) == "InProgress" && st(a) == "InProgress" && st(b) == "NotStarted");
        m.completeTask(a);
        CHECK("finishing A starts B", st(a) == "Complete" && st(b) == "InProgress" && st(c) == "NotStarted");
        m.completeTask(b); m.completeTask(c);
        CHECK("finishing the last task completes the root and the workflow", st(root) == "Complete" && m.isFinished());
        delete root;
    }

    SECTION("2. Retail return: Receipt, then Refund and Restock in parallel, then Close");
    {
        Employee pat("Pat", "any", 1);
        Subprocess *root = new Subprocess(), *par = new Subprocess();
        Task *rec = T("Receipt"), *ref = T("Refund"), *res = T("Restock"), *close = T("Close");
        par->add(ref); par->add(res);
        root->add(rec); root->add(par); root->add(close);
        EngineMediator m; m.setRoot(root); m.setStrategy(new LeastLoadedStrategy()); m.setCandidates({ &pat });
        m.setExecutionMode(par, ExecutionMode::Parallel);
        m.startWorkflow(); m.completeTask(rec);
        CHECK("both parallel tasks start together", st(ref) == "InProgress" && st(res) == "InProgress" && st(close) == "NotStarted");
        m.completeTask(ref);
        CHECK("one parallel task done: Close still waits", st(close) == "NotStarted");
        m.completeTask(res);
        CHECK("group completes and Close starts", st(par) == "Complete" && st(close) == "InProgress");
        m.completeTask(close);
        CHECK("workflow finishes", m.isFinished());
        delete root;
    }

    SECTION("3. RoleBased and MostExperience, swapped while running");
    {
        Employee anna("Anna", "HR", 2), ben("Ben", "HR", 9), cara("Cara", "IT", 5);
        Subprocess* root = new Subprocess();
        Task *t1 = T("Check ID", "IT"), *t2 = T("Pick anyone");
        root->add(t1); root->add(t2);
        EngineMediator m; m.setRoot(root); m.setStrategy(new RoleBasedStrategy()); m.setCandidates({ &anna, &ben, &cara });
        m.startWorkflow();
        CHECK("RoleBased gives the IT task to the IT person", who(t1) == "Cara");
        m.setStrategy(new MostExperienceStrategy());
        m.completeTask(t1);
        CHECK("after swapping to MostExperience the most experienced person is picked", who(t2) == "Ben");
        delete root;
    }

    SECTION("4. LeastLoaded spreads parallel work across people");
    {
        Employee anna("Anna", "HR", 2), ben("Ben", "HR", 9);
        Subprocess* root = new Subprocess();
        std::vector<Task*> ts;
        for (int i = 0; i < 4; i++) { ts.push_back(T("task" + std::to_string(i))); root->add(ts.back()); }
        EngineMediator m; m.setRoot(root); m.setStrategy(new LeastLoadedStrategy()); m.setCandidates({ &anna, &ben });
        m.setExecutionMode(root, ExecutionMode::Parallel);
        m.startWorkflow();
        CHECK("each person gets two of the four tasks", anna.getWorkload() == 2 && ben.getWorkload() == 2);
        CHECK("assignments alternate", who(ts[0]) == "Anna" && who(ts[1]) == "Ben" && who(ts[2]) == "Anna" && who(ts[3]) == "Ben");
        delete root;
    }

    SECTION("5. LeastLoaded rotates sequential work after completed tasks");
    {
        Employee anna("Anna", "HR", 2), ben("Ben", "HR", 9);
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *b = T("B"), *c = T("C"), *d = T("D");
        root->add(a); root->add(b); root->add(c); root->add(d);
        EngineMediator m; m.setRoot(root); m.setStrategy(new LeastLoadedStrategy()); m.setCandidates({ &anna, &ben });
        m.startWorkflow();
        m.completeTask(a);
        m.completeTask(b);
        m.completeTask(c);
        CHECK("completed tasks are still distributed across both employees",
              who(a) == "Anna" && who(b) == "Ben" && who(c) == "Anna" && who(d) == "Ben");
        delete root;
    }

    SECTION("6. RoleBased rotates when tasks have no specific role");
    {
        Employee anna("Anna", "HR", 2), ben("Ben", "HR", 9);
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *b = T("B"), *c = T("C");
        root->add(a); root->add(b); root->add(c);
        EngineMediator m; m.setRoot(root); m.setStrategy(new RoleBasedStrategy()); m.setCandidates({ &anna, &ben });
        m.startWorkflow();
        m.completeTask(a);
        m.completeTask(b);
        CHECK("role-compatible sequential work is rotated", who(a) == "Anna" && who(b) == "Ben" && who(c) == "Anna");
        delete root;
    }

    SECTION("7. Workload drops when work is finished");
    {
        Employee anna("Anna", "HR", 2);
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *b = T("B");
        root->add(a); root->add(b);
        EngineMediator m; m.setRoot(root); m.setStrategy(new LeastLoadedStrategy()); m.setCandidates({ &anna });
        m.startWorkflow();
        CHECK("one active task while A runs", anna.getWorkload() == 1);
        m.completeTask(a);
        CHECK("still one: B has taken A's place", anna.getWorkload() == 1);
        m.completeTask(b);
        CHECK("zero once everything is done", anna.getWorkload() == 0);
        delete root;
    }

    SECTION("8. Undoing an assignment gives the workload back");
    {
        Employee anna("Anna", "HR", 2);
        Subprocess* root = new Subprocess();
        Task* a = T("A"); root->add(a);
        EngineMediator m; m.setRoot(root); m.setStrategy(new LeastLoadedStrategy()); m.setCandidates({ &anna });
        m.startWorkflow();
        CHECK("assigned after start", who(a) == "Anna" && anna.getWorkload() == 1);
        m.undo(); m.undo(); // undo Start A, then Assign A
        CHECK("assignee and workload are restored", a->getAssignee() == nullptr && anna.getWorkload() == 0);
        delete root;
    }

    SECTION("9. Rejection halts the flow; undo resumes it");
    {
        Employee pat("Pat", "any", 1);
        Subprocess* root = new Subprocess();
        Task *a = T("A"), *b = T("B"), *c = T("C");
        root->add(a); root->add(b); root->add(c);
        EngineMediator m; m.setRoot(root); m.setStrategy(new LeastLoadedStrategy()); m.setCandidates({ &pat });
        m.startWorkflow(); m.completeTask(a); m.rejectTask(b);
        CHECK("B rejected: workflow halted and C never starts", m.isHalted() && st(c) == "NotStarted");
        CHECK("a rejected task no longer counts as workload", pat.getWorkload() == 0);
        CHECK("undo clears the halt", m.undo() && !m.isHalted() && st(b) == "InProgress");
        m.completeTask(b);
        CHECK("flow resumes", st(c) == "InProgress");
        delete root;
    }

    SECTION("10. A Department as the candidate pool");
    {
        Department dept("Support");
        dept.add(new Employee("Xi", "HR", 3)); dept.add(new Employee("Yo", "HR", 4));
        Subprocess* root = new Subprocess(); Task* t = T("Only task", "HR"); root->add(t);
        EngineMediator m; m.setRoot(root); m.setStrategy(new MostExperienceStrategy()); m.setCandidates({ &dept });
        m.startWorkflow();
        CHECK("department is flattened into its employees", who(t) == "Yo");
        delete root;
    }

    SECTION("11. Groups are never given to a person; no strategy and nobody suitable are safe");
    {
        Subprocess* root = new Subprocess(); Task* t = T("Needs legal", "Legal"); root->add(t);
        Employee pat("Pat", "HR", 1);
        EngineMediator m; m.setRoot(root); m.setCandidates({ &pat });
        m.startWorkflow(); // no strategy set
        CHECK("works without a strategy", st(t) == "InProgress" && t->getAssignee() == nullptr);
        delete root;
        Subprocess* root2 = new Subprocess(); Task* t2 = T("Needs legal", "Legal"); root2->add(t2);
        EngineMediator m2; m2.setRoot(root2); m2.setStrategy(new RoleBasedStrategy()); m2.setCandidates({ &pat });
        m2.startWorkflow();
        CHECK("nobody has the role: task starts unassigned", t2->getAssignee() == nullptr);
        delete root2;
    }

    SECTION("12. Facade smoke test: WorkflowDefinition, spawn, run to the end");
    {
        WorkflowEngine engine;
        bool ok = engine.assembleBlueprint("Retail Return", 1, { "Service" },
            { { "Lerato", "Service", "Clerk", 2 }, { "Sipho", "Service", "Manager", 9 } },
            { { "Phase", "ROOT" } },
            { { "Scan", "ROOT", 1, "" }, { "Inspect", "Phase", 1, "" }, { "Restock", "Phase", 2, "" } });
        CHECK("WorkflowDefinition assembled", ok);
        int id = engine.instantiateInstance("Retail Return", "MostExperience");
        CHECK("instance spawned", id >= 0);
        CHECK("instance starts", engine.startInstance(id) && engine.getInstanceStatus(id) == "Running");
        std::vector<TaskRow> initialRows = engine.getTaskRows(id);
        bool assignedToMostExperienced = false;
        for (const TaskRow &row : initialRows)
            if (!row.isGroup && row.name == "Inspect" && row.assignee == "Sipho")
                assignedToMostExperienced = true;
        CHECK("builder preserves employee experience for MostExperience assignment", assignedToMostExperienced);
        for (int guard = 0; guard < 10 && engine.getInstanceStatus(id) != "Finished"; ++guard) {
            std::vector<TaskRow> rows = engine.getTaskRows(id);
            for (size_t i = 0; i < rows.size(); ++i)
                if (!rows[i].isGroup && rows[i].state == "InProgress") { engine.completeTask(id, (int)i); break; }
        }
        CHECK("instance runs to Finished", engine.getInstanceStatus(id) == "Finished");
    }

    std::cout.rdbuf(engineChatter);
    std::cerr << "\n" << (total - failed) << "/" << total << " checks passed\n";
    return failed ? 1 : 0;
}

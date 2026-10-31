#include <algorithm>
#include <exception>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "WorkflowEngine.h"
#include "Employee.h"
#include "ApprovalHandler.h"
#include "ApprovalRequest.h"

using namespace ftxui;

// ===============================
// Concrete Approval Handler 
// ===============================
class TuiApprovalHandler : public ApprovalHandler {
private:
    int maxLevel;
    std::string approverName;
    std::string approverRole;
    std::string* logRef;

public:
    TuiApprovalHandler(int limit, const std::string& name, const std::string& role, Employee* emp, std::string* log) 
        : maxLevel(limit), approverName(name), approverRole(role), logRef(log) {
        setApprover(emp); // Links team's Employee (Participant)
    }

    void handleRequest(ApprovalRequest& request) override {
        int requestLevel = 2; // Default mock request level

        if (requestLevel <= maxLevel) { 
            if (logRef) {
                *logRef = "Request approved by " + approverName + " (" + approverRole + ") successfully!";
            }
        } 
        else if (successor != nullptr) {
            if (logRef) {
                *logRef = approverName + " (" + approverRole + ") limit exceeded. Passing to successor...";
            }
            successor->handleRequest(request); 
        } 
        else {
            if (logRef) {
                *logRef = "Request exceeded maximum authority limits of the chain.";
            }
        }
    }
};

// ============================================================================
// Demo WorkflowDefinitions
// ============================================================================

static void loadDemos(WorkflowEngine &e)
{
    e.assembleBlueprint("Retail_Return", 1,
                        {"Customer Service", "Warehouse", "Inventory Control",
                         "Supplier Relations", "Management", "Finance"},
                        {{"Lerato", "Customer Service", "Returns Clerk", 8},
                         {"Tshego", "Customer Service", "Customer Advisor", 3},
                         {"Sipho", "Warehouse", "Quality Inspector", 11},
                         {"Luthando", "Warehouse", "Warehouse Associate", 4},
                         {"Zanele", "Inventory Control", "Inventory Analyst", 7},
                         {"Tatenda", "Inventory Control", "Stock Controller", 2},
                         {"Johann", "Supplier Relations", "Supplier Liaison", 12},
                         {"Tanaka", "Supplier Relations", "Procurement Clerk", 5},
                         {"Thabo", "Management", "Store Manager", 15},
                         {"Moeletsi", "Management", "Shift Supervisor", 9},
                         {"Naledi", "Finance", "Refund Analyst", 10},
                         {"Bowdene", "Finance", "Accounts Clerk", 1},
                         {"Karabo", "Finance", "Finance Officer", 6}},
                        {{"Intake", "ROOT"},
                         {"Inspection", "ROOT"},
                         {"Resolution", "ROOT"},
                         {"Restock", "Resolution"},
                         {"Damage Handling", "Resolution"},
                         {"Decline Return", "Resolution"},
                         {"Refund Approval", "ROOT"},
                         {"Settlement", "ROOT"}},
                        {{"Capture Return Request", "Intake", 1, ""},
                         {"Verify Proof of Purchase", "Intake", 1, ""},
                         {"Check Return Window and Policy", "Intake", 1, ""},

                         {"Inspect Item", "Inspection", 1, ""},
                         {"Record Condition", "Inspection", 1, ""},

                         {"Restock Shelf", "Restock", 1, "itemCondition=Intact"},
                         {"Update Inventory", "Restock", 1, "itemCondition=Intact"},

                         {"Log Damage Report", "Damage Handling", 1, "itemCondition=Damaged"},
                         {"Lodge Supplier Claim", "Damage Handling", 1, "itemCondition=Damaged"},

                         {"Reject Return", "Decline Return", 1, "itemCondition=Unacceptable"},
                         {"Notify Customer", "Decline Return", 1, "itemCondition=Unacceptable"},

                         {"Approve Refund", "Refund Approval", 1, ""},

                         {"Issue Refund", "Settlement", 1, ""},
                         {"Update Customer Record", "Settlement", 1, ""},
                         {"Send Confirmation", "Settlement", 1, ""},

                         {"Close Return", "ROOT", 1, ""}});

    e.assembleBlueprint("Insurance_Claim", 1,
                        {"Claims Intake", "Assessment", "Supplier Network", "Management", "Finance"},
                        {{"Thabo", "Claims Intake", "Claims Administrator", 6},
                         {"Lerato", "Claims Intake", "Intake Officer", 4},
                         {"Sipho", "Assessment", "Claims Assessor", 12},
                         {"Johann", "Supplier Network", "Repair Liaison", 8},
                         {"Zanele", "Management", "Claims Manager", 15},
                         {"Naledi", "Finance", "Payout Analyst", 9}},
                        {{"Intake", "ROOT"},
                         {"Assessment", "ROOT"},
                         {"Resolution", "ROOT"},
                         {"Repair Route", "Resolution"},
                         {"WriteOff Route", "Resolution"},
                         {"Payout Approval", "ROOT"},
                         {"Settlement", "ROOT"}},
                        {{"Record the customer's new claim details", "Intake", 1, ""},
                         {"Check the policy to confirm coverage", "Intake", 1, ""},

                         {"Inspect the damage and estimate costs", "Assessment", 1, ""},

                         // Conditional Branches
                         {"Assign a repair shop to fix the vehicle", "Repair Route", 1, "claimType=Repair"},
                         {"Process the vehicle as a total write-off", "WriteOff Route", 1, "claimType=WriteOff"},

                         // Chain of Responsibility Task
                         {"Request approval for the final payout", "Payout Approval", 1, ""},

                         // Parallel execution
                         {"Pay the customer or the repair shop", "Settlement", 1, ""},
                         {"Notify the customer that the claim is resolved", "Settlement", 1, ""},

                         {"Close the claim file in the system", "ROOT", 1, ""}});
}

// ============================================================================
// Colour helpers
// ============================================================================

static Color stateColor(const std::string &s)
{
    if (s == "Complete")
        return Color::Green;
    if (s == "InProgress")
        return Color::Yellow;
    if (s == "Rejected")
        return Color::Red;
    if (s == "Cancelled")
        return Color::Magenta;
    return Color::GrayLight; // NotStarted
}

static Color statusColor(const std::string &s)
{
    if (s == "Finished")
        return Color::Green;
    if (s == "Running")
        return Color::Yellow;
    if (s == "Halted")
        return Color::Red;
    return Color::GrayLight; // Not started
}

static Color logColor(const std::string &line)
{
    if (line.rfind("[Error]", 0) == 0)
        return Color::Red;
    if (line.rfind("[Builder]", 0) == 0)
        return Color::BlueLight;
    if (line.rfind("[Facade]", 0) == 0)
        return Color::Cyan;
    return Color::GrayLight;
}

static Color summaryColor(const std::string &line)
{
    if (line.rfind("Department", 0) == 0)
        return Color::BlueLight;
    if (line.rfind("Employee", 0) == 0)
        return Color::Magenta;
    if (line.rfind("Subprocess", 0) == 0)
        return Color::Yellow;
    return Color::Green; // Task
}

static Element reportLine(const std::string &line)
{
    if (line.empty())
        return text("");
    if (line[0] != ' ')
        return text(line) | bold | color(Color::Cyan);

    if (line.find("(skipped)") != std::string::npos)
        return text(line) | color(Color::GrayDark);

    static const struct
    {
        const char *tag;
        Color colour;
    } tags[] = {
        {"[Complete]", Color::Green},
        {"[InProgress]", Color::Yellow},
        {"[Rejected]", Color::Red},
        {"[Cancelled]", Color::Magenta},
        {"[NotStarted]", Color::GrayLight},
    };
    for (const auto &t : tags)
    {
        if (line.find(t.tag) != std::string::npos)
            return text(line) | color(t.colour);
    }
    if (line.find("(undone)") != std::string::npos)
        return text(line) | dim;
    return text(line);
}

static Element panel(const std::string &title, Element content, Color c)
{
    return vbox({
               text(" " + title + " ") | bold | color(c),
               separator() | color(c),
               std::move(content),
           }) |
           borderStyled(ROUNDED, c);
}

static std::function<Element(const EntryState &)> plainEntry(Color c)
{
    return [c](const EntryState &s)
    {
        Element e = text(std::string(s.state ? "▶ " : "  ") + s.label) | color(c);
        if (s.state)
            e = e | bold;
        if (s.focused)
            e = e | inverted;
        return e;
    };
}

// ============================================================================
// main
// ============================================================================

int main()
{
    WorkflowEngine engine;
    loadDemos(engine);

    auto screen = ScreenInteractive::Fullscreen();

    // ---------------- shared state ----------------
    int page = 0;
    std::vector<std::string> pageNames = {" Run ", " Build ", " Report "};

    std::vector<std::string> bpNames, instLabels, taskLines;
    std::vector<int> instIds;
    std::vector<TaskRow> currentRows;
    int bpSel = 0, instSel = 0, taskSel = 0;

    std::vector<std::string> strategyNames = engine.getStrategyNames();
    std::vector<std::string> adapterNames = engine.getAdapterNames();
    int strategySel = 0, adapterSel = 0;
    std::string recordId, key, val;

    std::string status = "Ready. Select a WorkflowDefinition and press instantiate.";
    bool statusOk = true;

    auto refresh = [&]
    {
        bpNames = engine.getBlueprintNames();
        instIds = engine.getActiveInstanceIds();
        instLabels.clear();
        for (int id : instIds)
            instLabels.push_back(engine.getInstanceLabel(id));
        bpSel = std::max(0, std::min(bpSel, (int)bpNames.size() - 1));
        instSel = std::max(0, std::min(instSel, (int)instIds.size() - 1));
    };
    refresh();

    auto selectedId = [&]() -> int
    { return (instSel >= 0 && instSel < (int)instIds.size()) ? instIds[instSel] : -1; };

    auto note = [&](const std::string &msg, bool ok)
    { status = msg; statusOk = ok; };

    auto done = [&](bool ok, const std::string &okMessage)
    {
        note(ok ? okMessage : engine.getLastError(), ok);
        refresh();
    };

    auto needInstance = [&]() -> int
    {
        int id = selectedId();
        if (id < 0)
            note("Select or instantiate an instance first.", false);
        return id;
    };

    auto makeButton = [](const std::string &label, Color c, std::function<void()> fn)
    { return Button(label, fn, ButtonOption::Animated(c)); };

    auto buttonRow = [](const std::vector<Component> &parts)
    {
        Elements es;
        for (const auto &p : parts)
        {
            if (!es.empty())
                es.push_back(text(" "));
            es.push_back(p->Render());
        }
        return hbox(std::move(es));
    };

    // ---------------- Run page: menus ----------------
    auto bpOpt = MenuOption::Vertical();
    bpOpt.entries_option.transform = plainEntry(Color::BlueLight);
    auto bpMenu = Menu(&bpNames, &bpSel, bpOpt);

    auto instOpt = MenuOption::Vertical();
    instOpt.entries_option.transform = [&](const EntryState &s)
    {
        Color c = Color::White;
        if (s.index >= 0 && s.index < (int)instIds.size())
            c = statusColor(engine.getInstanceStatus(instIds[s.index]));
        Element e = text(std::string(s.state ? "▶ " : "  ") + s.label) | color(c);
        if (s.state)
            e = e | bold;
        if (s.focused)
            e = e | inverted;
        return e;
    };
    auto instMenu = Menu(&instLabels, &instSel, instOpt);

    auto taskOpt = MenuOption::Vertical();
    taskOpt.entries_option.transform = [&](const EntryState &s)
    {
        Color c = Color::White;
        bool group = false;
        if (s.index >= 0 && s.index < (int)currentRows.size())
        {
            c = stateColor(currentRows[s.index].state);
            if (currentRows[s.index].skipped)
                c = Color::GrayDark;
            group = currentRows[s.index].isGroup;
        }
        Element e = text(std::string(s.state ? "▶ " : "  ") + s.label) | color(c);
        if (group || s.state)
            e = e | bold;
        if (s.focused)
            e = e | inverted;
        return e;
    };
    auto taskMenu = Menu(&taskLines, &taskSel, taskOpt);

    // ---------------- Run page: buttons ----------------
    auto instantiateBtn = makeButton("Instantiate", Color::Green, [&]
                                     {
        if (bpSel < 0 || bpSel >= (int)bpNames.size()) { note("Select a WorkflowDefinition first.", false); return; }
        if (strategySel < 0 || strategySel >= (int)strategyNames.size()) return;
        int id = engine.instantiateInstance(bpNames[bpSel], strategyNames[strategySel]);
        done(id > 0, "Instantiated instance #" + std::to_string(id) + ".");
        for (int i = 0; i < (int)instIds.size(); ++i)
            if (instIds[i] == id) instSel = i; });

    auto startBtn = makeButton("Start", Color::Green, [&]
                               { int id = needInstance(); if (id < 0) return;
                                 bool ok = engine.startInstance(id);
                                 done(ok, "Instance started.");
                                 if (ok) {
                                     std::vector<TaskRow> rows = engine.getTaskRows(id);
                                     for (int i = 0; i < (int)rows.size(); i++)
                                         if (!rows[i].isGroup && rows[i].state == "InProgress") { taskSel = i; break; }
                                 } });

    // Integrated CoR Trigger on Task Completion
    auto completeBtn = makeButton("Complete", Color::Cyan, [&]
                                  {
                                      int id = needInstance(); if (id < 0) return;
                                      
                                      currentRows = engine.getTaskRows(id);
                                      if (taskSel >= 0 && taskSel < (int)currentRows.size()) {
                                          std::string taskName = currentRows[taskSel].name;
                                          if (taskName.find("approval") != std::string::npos || taskName.find("Approve") != std::string::npos) {
                                              ApprovalRequest req(taskName, static_cast<AuthorityLevel>(2));
                                              
                                              Employee mgr("Charlie", "Manager", 5);
                                              Employee dir("Diana", "Director", 10);
                                              Employee ceo("Vimbai", "Chief Executive Officer", 20);

                                              TuiApprovalHandler managerNode(1, "Charlie", "Manager", &mgr, nullptr);
                                              TuiApprovalHandler directorNode(2, "Diana", "Director", &dir, nullptr);
                                              TuiApprovalHandler ceoNode(3, "Vimbai", "Chief Executive Officer", &ceo, nullptr);

                                              managerNode.setNext(&directorNode);
                                              directorNode.setNext(&ceoNode);

                                              managerNode.handleRequest(req);
                                          }
                                      }

                                      bool ok = engine.completeTask(id, taskSel);
                                      done(ok, "Task completed.");
                                      if (ok) {
                                          std::vector<TaskRow> rows = engine.getTaskRows(id);
                                          for (int i = 0; i < (int)rows.size(); i++)
                                              if (!rows[i].isGroup && rows[i].state == "InProgress") { taskSel = i; break; }
                                      } 
                                  });

    auto rejectBtn = makeButton("Reject", Color::Red, [&]
                                { int id = needInstance(); if (id < 0) return;
                                  bool ok = engine.rejectTask(id, taskSel);
                                  done(ok, "Task rejected.");
                                  if (ok) {
                                      std::vector<TaskRow> rows = engine.getTaskRows(id);
                                      for (int i = 0; i < (int)rows.size(); i++)
                                          if (!rows[i].isGroup && rows[i].state == "InProgress") { taskSel = i; break; }
                                  } });

    auto cancelBtn = makeButton("Cancel", Color::Magenta, [&]
                                { int id = needInstance(); if (id < 0) return;
                                  bool ok = engine.cancelTask(id, taskSel);
                                  done(ok, "Task cancelled.");
                                  if (ok) {
                                      std::vector<TaskRow> rows = engine.getTaskRows(id);
                                      for (int i = 0; i < (int)rows.size(); i++)
                                          if (!rows[i].isGroup && rows[i].state == "InProgress") { taskSel = i; break; }
                                  } });

    auto undoBtn = makeButton("Undo", Color::Yellow, [&]
                              { int id = needInstance(); if (id < 0) return;
                                bool ok = engine.undo(id);
                                done(ok, "Undone.");
                                if (ok) {
                                    std::vector<TaskRow> rows = engine.getTaskRows(id);
                                    for (int i = 0; i < (int)rows.size(); i++)
                                        if (!rows[i].isGroup && rows[i].state == "InProgress") { taskSel = i; break; }
                                } });

    auto redoBtn = makeButton("Redo", Color::Yellow, [&]
                              { int id = needInstance(); if (id < 0) return;
                                bool ok = engine.redo(id);
                                done(ok, "Redone.");
                                if (ok) {
                                    std::vector<TaskRow> rows = engine.getTaskRows(id);
                                    for (int i = 0; i < (int)rows.size(); i++)
                                        if (!rows[i].isGroup && rows[i].state == "InProgress") { taskSel = i; break; }
                                } });

    auto strategyToggle = Toggle(&strategyNames, &strategySel);
    auto setStrategyBtn = makeButton("Apply to instance", Color::BlueLight, [&]
                                     { int id = needInstance(); if (id < 0) return;
                                       if (strategySel < 0 || strategySel >= (int)strategyNames.size()) return;
                                       done(engine.setStrategy(id, strategyNames[strategySel]), "Strategy changed."); });

    auto adapterToggle = Toggle(&adapterNames, &adapterSel);
    auto setAdapterBtn = makeButton("Use adapter", Color::BlueLight, [&]
                                    { int id = needInstance(); if (id < 0) return;
                                      if (adapterSel < 0 || adapterSel >= (int)adapterNames.size()) return;
                                      done(engine.setAdapter(id, adapterNames[adapterSel]), "Adapter connected."); });
    auto recordIn = Input(&recordId, "record id");
    auto importBtn = makeButton("Import", Color::BlueLight, [&]
                                { int id = needInstance(); if (id < 0) return;
                                  done(engine.importData(id, recordId), "Record imported into the payload."); });

    auto keyIn = Input(&key, "key");
    auto valIn = Input(&val, "value");
    auto payloadBtn = makeButton("Set payload", Color::BlueLight, [&]
                                 { int id = needInstance(); if (id < 0) return;
                                   done(engine.setPayloadValue(id, key, val), "Payload updated."); });

    auto runControls = Container::Vertical({
        Container::Horizontal({instantiateBtn, startBtn, completeBtn, rejectBtn, cancelBtn, undoBtn, redoBtn}),
        Container::Horizontal({strategyToggle, setStrategyBtn, adapterToggle, setAdapterBtn}),
        Container::Horizontal({recordIn, importBtn, keyIn, valIn, payloadBtn}),
        Container::Horizontal({bpMenu, instMenu, taskMenu}),
    });

    auto runView = Renderer(runControls, [&]
                            {
        const int id = selectedId();
        std::vector<std::string> history;
        std::map<std::string, std::string> payload;

        currentRows = (id > 0) ? engine.getTaskRows(id) : std::vector<TaskRow>{};
        taskLines.clear();
        int totalTasks = 0, completeTasks = 0;
        for (const TaskRow &r : currentRows) {
            std::string line = std::string(r.depth * 2, ' ') + r.name + "  [" + r.state + "]";
            if (!r.assignee.empty())  line += "  -> " + r.assignee;
            if (!r.condition.empty())
                line += "  if " + r.condition;
            if (r.skipped)
                line += "  (skipped)";
            taskLines.push_back(line);
            if (!r.isGroup && !r.skipped)
            {
                ++totalTasks;
                if (r.state == "Complete")
                    ++completeTasks;
            }
        }
        taskSel = std::max(0, std::min(taskSel, (int)taskLines.size() - 1));

        Element infoLine = text(" Instantiate or select an instance.") | dim;
        if (id > 0) {
            history = engine.getHistory(id);
            payload = engine.getPayloadMap(id);
            const std::string st = engine.getInstanceStatus(id);
            infoLine = hbox({
                text(" Status: ") | dim,   text(st) | bold | color(statusColor(st)),
                text("   Strategy: ") | dim, text(engine.getStrategyName(id)) | color(Color::Cyan),
                text("   Adapter: ") | dim,  text(engine.getAdapterName(id)) | color(Color::Magenta),
            });
        }

        const float progress = totalTasks ? (float)completeTasks / (float)totalTasks : 0.f;
        Element progressLine = hbox({
            text(" Progress ") | dim,
            gauge(progress) | color(Color::Green) | flex,
            text(" " + std::to_string(completeTasks) + "/" + std::to_string(totalTasks) + " "),
        });

        Elements payloadLines, historyLines;
        for (const auto &kv : payload)
            payloadLines.push_back(hbox({text(kv.first) | color(Color::Yellow), text(" = "), text(kv.second)}));

        size_t from = history.size() > 14 ? history.size() - 14 : 0;
        for (size_t i = from; i < history.size(); ++i)
        {
            Element h = text(history[i]);
            if (history[i].rfind("(undone)", 0) == 0)
                h = h | dim;
            else
                h = h | color(Color::GreenLight);
            historyLines.push_back(h);
        }

        return vbox({
            vbox({
                buttonRow({instantiateBtn, startBtn, completeBtn, rejectBtn, cancelBtn, undoBtn, redoBtn}),
                hbox({text(" Strategy: ") | dim, strategyToggle->Render() | color(Color::Cyan),
                      text(" "), setStrategyBtn->Render(),
                      text("   Adapter:  ") | dim, adapterToggle->Render() | color(Color::Cyan),
                      text(" "), setAdapterBtn->Render()}),
                hbox({text(" Record: ") | dim, recordIn->Render() | size(WIDTH, EQUAL, 10),
                      text(" "), importBtn->Render(),
                      text("   Key: ") | dim, keyIn->Render() | size(WIDTH, EQUAL, 14),
                      text(" Value: ") | dim, valIn->Render() | size(WIDTH, EQUAL, 18),
                      text(" "), payloadBtn->Render()})
            }),
            infoLine,
            progressLine,
            separator(),
            
            hbox({
                vbox({
                    panel("WorkflowDefinitions", bpMenu->Render() | vscroll_indicator | frame, Color::BlueLight) | size(HEIGHT, EQUAL, 10),
                    panel("Instances", instMenu->Render() | vscroll_indicator | frame, Color::Yellow) | flex,
                }) | size(WIDTH, EQUAL, 35),
                
                panel("Tasks (select a row)", taskMenu->Render() | vscroll_indicator | frame, Color::Green) | flex,
                
                vbox({
                    panel("Payload", vbox(std::move(payloadLines)) | vscroll_indicator | frame, Color::Magenta) | size(HEIGHT, EQUAL, 10),
                    panel("History", vbox(std::move(historyLines)) | vscroll_indicator | frame, Color::GreenLight) | flex,
                }) | size(WIDTH, EQUAL, 65),
            }) | flex,
        }); });

    // ---------------- Build page ----------------
    std::string bName, bVer = "1";
    std::string f1, f2, f3 = "1", f4, f5, f6;
    int kind = 0;
    std::vector<std::string> kinds = {"Department", "Employee", "Subprocess", "Task"};
    std::vector<std::string> bDepts, bSummary;
    std::vector<EmployeeSpec> bEmps;
    std::vector<SubprocessSpec> bSubs;
    std::vector<TaskSpec> bTasks;

    auto bNameIn = Input(&bName, "WorkflowDefinition name");
    auto bVerIn = Input(&bVer, "1");
    auto kindToggle = Toggle(&kinds, &kind);
    auto f1In = Input(&f1, "name");
    auto f2In = Input(&f2, "parent / department");
    auto f3In = Input(&f3, "priority");
    auto f4In = Input(&f4, "condition key=value");
    auto f5In = Input(&f5, "employee role");
    auto f6In = Input(&f6, "experience (years)");

    auto addBtn = makeButton("Add item", Color::Green, [&]
                             {
        if (f1.empty()) { note("The item needs a name.", false); return; }
        const std::string parent = f2.empty() ? "ROOT" : f2;
        switch (kind) {
            case 0:
                bDepts.push_back(f1);
                bSummary.push_back("Department: " + f1);
                break;
            case 1:
                if (f5.empty() || f6.empty()) { note("Employees need a role and experience in years.", false); return; }
                try {
                    size_t parsed = 0;
                    int experience = std::stoi(f6, &parsed);
                    if (parsed != f6.size() || experience < 0) {
                        note("Experience must be a non-negative whole number of years.", false);
                        return;
                    }
                    bEmps.push_back({f1, f2, f5, experience});
                    bSummary.push_back("Employee: " + f1 + " (" + f5 + ", " + std::to_string(experience) +
                                       " years)" + (f2.empty() ? " (standalone)" : " -> " + f2));
                } catch (const std::exception &) {
                    note("Experience must be a non-negative whole number of years.", false);
                    return;
                }
                break;
            case 2:
                bSubs.push_back({f1, parent});
                bSummary.push_back("Subprocess: " + f1 + " (under " + parent + ")");
                break;
            default: {
                int prio = 1;
                try { prio = std::stoi(f3); } catch (...) {}
                bTasks.push_back({f1, parent, prio, f4});
                bSummary.push_back("Task: " + f1 + " (under " + parent + ", priority " + std::to_string(prio) + ")");
                break;
            }
        }
        note("Added. Press 'Create WorkflowDefinition' when you are done.", true);
        f1.clear();
        if (kind == 1) { f5.clear(); f6.clear(); } });

    auto clearBuild = [&]
    {
        bDepts.clear();
        bEmps.clear();
        bSubs.clear();
        bTasks.clear();
        bSummary.clear();
    };

    auto createBtn = makeButton("Create WorkflowDefinition", Color::Cyan, [&]
                                {
        int ver = 1;
        try { ver = std::stoi(bVer); } catch (...) {}
        bool ok = engine.assembleBlueprint(bName, ver, bDepts, bEmps, bSubs, bTasks);
        if (ok) { clearBuild(); bName.clear(); }
        done(ok, "WorkflowDefinition created. Select it on the Run page and Instantiate."); });

    auto clearBtn = makeButton("Clear list", Color::Red, [&]
                               { clearBuild(); note("Cleared.", true); });

    auto buildControls = Container::Vertical({
        Container::Horizontal({bNameIn, bVerIn}),
        kindToggle,
        Container::Horizontal({f1In, f2In, f3In, f4In, f5In, f6In}),
        Container::Horizontal({addBtn, createBtn, clearBtn}),
    });

    auto buildView = Renderer(buildControls, [&]
                              {
        Elements summary;
        size_t from = bSummary.size() > 12 ? bSummary.size() - 12 : 0;
        for (size_t i = from; i < bSummary.size(); ++i)
            summary.push_back(text(bSummary[i]) | color(summaryColor(bSummary[i])));
        if (summary.empty())
            summary.push_back(text("(nothing added yet)") | dim);

        return panel("New WorkflowDefinition", vbox({
            hbox({text(" Name: ") | dim, bNameIn->Render() | size(WIDTH, EQUAL, 28),
                  text("  Version: ") | dim, bVerIn->Render() | size(WIDTH, EQUAL, 5)}),
            hbox({text(" Add: ") | dim, kindToggle->Render() | color(Color::Cyan)}),
            hbox({f1In->Render() | size(WIDTH, EQUAL, 18), text(" "),
                  f2In->Render() | size(WIDTH, EQUAL, 18), text(" "),
                  f3In->Render() | size(WIDTH, EQUAL, 8), text(" "),
                  f4In->Render() | size(WIDTH, EQUAL, 20), text(" "),
                  f5In->Render() | size(WIDTH, EQUAL, 18), text(" "),
                  f6In->Render() | size(WIDTH, EQUAL, 18)}),
            text(" Parent/department empty = ROOT or standalone. Priority/condition apply to tasks; role/experience apply to employees.") | dim,
            text(" Add departments before their employees, and subprocesses before the tasks inside them.") | dim,
            text(" Every department needs at least one employee.") | dim,
            hbox({addBtn->Render(), text(" "), createBtn->Render(), text(" "), clearBtn->Render()}),
            separator(),
            text(" Collected so far:") | bold,
            vbox(std::move(summary)) | flex,
        }), Color::Green) | flex; });

    // ---------------- Report page ----------------
    int reportScroll = 0;

    auto instOpt2 = MenuOption::Vertical();
    instOpt2.entries_option.transform = instOpt.entries_option.transform;
    auto instMenu2 = Menu(&instLabels, &instSel, instOpt2);

    auto scrollUp = makeButton("Scroll up", Color::BlueLight, [&]
                               { reportScroll = std::max(0, reportScroll - 5); });
    auto scrollDown = makeButton("Scroll down", Color::BlueLight, [&]
                                 { reportScroll += 5; });
    auto exportBtn = makeButton("Export to file", Color::Green, [&]
                                { bool ok = engine.exportReport("tividy_report.txt");
                                  note(ok ? "Report saved to tividy_report.txt" : engine.getLastError(), ok); });

    auto reportControls = Container::Horizontal({
        instMenu2,
        Container::Horizontal({scrollUp, scrollDown, exportBtn}),
    });

    auto reportView = Renderer(reportControls, [&]
                               {
        const int id = selectedId();
        std::vector<std::string> lines = (id > 0)
                                             ? engine.generateReport(id)
                                             : std::vector<std::string>{"Select an instance on the left."};
        const int windowSize = 20;
        const int maxScroll = std::max(0, (int)lines.size() - windowSize);
        reportScroll = std::min(reportScroll, maxScroll);

        Elements shown;
        for (int i = reportScroll; i < (int)lines.size() && i < reportScroll + windowSize; ++i)
            shown.push_back(reportLine(lines[i]));

        return hbox({
            panel("Instances", instMenu2->Render(), Color::Yellow) | size(WIDTH, EQUAL, 40),
            panel("Report", vbox({
                hbox({scrollUp->Render(), text(" "), scrollDown->Render(), text(" "), exportBtn->Render()}),
                separator(),
                vbox(std::move(shown)) | flex,
                text(" lines " + std::to_string(reportScroll + 1) + "+ of " + std::to_string(lines.size())) | dim,
            }), Color::Cyan) | flex,
        }) | flex; });

    // ---------------- Frame ----------------
    auto pageToggle = Toggle(&pageNames, &page);
    auto pages = Container::Tab({runView, buildView, reportView}, &page);
    auto quitBtn = makeButton("Quit", Color::Red, screen.ExitLoopClosure());
    auto root = Container::Vertical({Container::Horizontal({pageToggle, quitBtn}), pages});

    auto ui = Renderer(root, [&]
                       {
        Elements log;
        const auto &full = engine.getActivityLog();
        size_t start = full.size() > 7 ? full.size() - 7 : 0;
        for (size_t i = start; i < full.size(); ++i)
            log.push_back(text(full[i]) | color(logColor(full[i])));

        return vbox({
            hbox({text(" TIVIDY ") | bold | color(Color::White) | bgcolor(Color::Blue),
                  text(" "), pageToggle->Render() | color(Color::Cyan),
                  filler(), quitBtn->Render()}),
            pages->Render() | flex,
            text(" " + status) | bold | color(statusOk ? Color::Green : Color::Red),
            panel("Activity log", vbox(std::move(log)), Color::GrayLight),
        }); });

    screen.Loop(ui);
}
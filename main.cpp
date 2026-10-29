#include <algorithm>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <map>
#include <stdexcept>
#include "WorkflowEngine.h"

using namespace ftxui;

static void loadDemos(WorkflowEngine &e)
{
  e.assembleBlueprint("Retail_Return_Item", 1,
                      {"Customer Service", "Warehouse"},
                      {{"Lerato", "Customer Service"}, {"Sipho", "Warehouse"}, {"Thabo", ""}},
                      {{"Inspection", "ROOT"}, {"Settlement", "ROOT"}},
                      {{"Scan Receipt", "ROOT", 1, ""},
                       {"Inspect Garment", "Inspection", 1, ""},
                       {"Restock Shelf", "Inspection", 2, "itemCondition=Intact"},
                       {"Approve Refund", "Settlement", 1, ""}});

  e.assembleBlueprint("Bank_Process_Loan", 1,
                      {"Credit Underwriting"},
                      {{"Naledi", "Credit Underwriting"}},
                      {{"Credit_Assessment", "ROOT"}},
                      {{"Verify FICA", "ROOT", 1, ""},
                       {"Credit Bureau Check", "Credit_Assessment", 1, ""},
                       {"Sign-Off", "Credit_Assessment", 2, "creditStatus=Approved"}});
}

int main()
{
  WorkflowEngine engine;
  loadDemos(engine);

  auto screen = ScreenInteractive::Fullscreen();

  // ---------------- shared state ----------------
  int page = 0;
  std::vector<std::string> pageNames = {"Run", "Build", "Report"};

  std::vector<std::string> bpNames, instLabels, taskLines;
  std::vector<int> instIds;
  int bpSel = 0, instSel = 0, taskSel = 0;

  std::vector<std::string> strategyNames = engine.getStrategyNames();
  std::vector<std::string> adapterNames = engine.getAdapterNames();
  int strategySel = 0, adapterSel = 0;
  std::string recordId, key, val;
  std::string status = "Ready.";

  auto refresh = [&]
  {
    bpNames = engine.getBlueprintNames();
    instIds = engine.getActiveInstanceIds();
    instLabels.clear();
    for (int id : instIds)
      instLabels.push_back(engine.getInstanceLabel(id));
    if (bpSel >= (int)bpNames.size())
      bpSel = bpNames.empty() ? 0 : (int)bpNames.size() - 1;
    if (instSel >= (int)instIds.size())
      instSel = instIds.empty() ? 0 : (int)instIds.size() - 1;
  };
  refresh();

  // Selected instance ID, or -1 if there is none.
  auto selectedId = [&]() -> int
  { return (instSel >= 0 && instSel < (int)instIds.size()) ? instIds[instSel] : -1; };

  // Show the result of a facade call and refresh the menus.
  auto done = [&](bool ok, const std::string &okMessage)
  {
    status = ok ? okMessage : engine.getLastError();
    refresh();
  };

  // ---------------- Run page ----------------
  auto bpMenu = Menu(&bpNames, &bpSel);
  auto instMenu = Menu(&instLabels, &instSel);
  auto taskMenu = Menu(&taskLines, &taskSel);

  auto spawnBtn = Button("Spawn", [&]
                         {
        if (bpSel < 0 || bpSel >= (int)bpNames.size()) { status = "Select a blueprint first."; return; }
        if (strategySel < 0) return;
        int id = engine.spawnInstance(bpNames[bpSel], strategyNames[strategySel]);
        done(id > 0, "Spawned instance #" + std::to_string(id) + ".");
        for (int i = 0; i < (int)instIds.size(); ++i)
            if (instIds[i] == id) instSel = i; });

  auto startBtn = Button("Start", [&]
                         { int id = selectedId(); if (id < 0) { status = "Select an instance."; return; }
                             done(engine.startInstance(id), "Instance started."); });
  auto nextBtn = Button("Next step", [&]
                        { int id = selectedId(); if (id < 0) { status = "Select an instance."; return; }
                            done(engine.completeNextTask(id), "Completed the task in progress."); });
  auto completeBtn = Button("Complete", [&]
                            { int id = selectedId(); if (id < 0) { status = "Select an instance."; return; }
                                done(engine.completeTask(id, taskSel), "Task completed."); });
  auto rejectBtn = Button("Reject", [&]
                          { int id = selectedId(); if (id < 0) { status = "Select an instance."; return; }
                              done(engine.rejectTask(id, taskSel), "Task rejected."); });
  auto cancelBtn = Button("Cancel", [&]
                          { int id = selectedId(); if (id < 0) { status = "Select an instance."; return; }
                              done(engine.cancelTask(id, taskSel), "Task cancelled."); });
  auto undoBtn = Button("Undo", [&]
                        { int id = selectedId(); if (id < 0) { status = "Select an instance."; return; }
                            done(engine.undo(id), "Undone."); });
  auto redoBtn = Button("Redo", [&]
                        { int id = selectedId(); if (id < 0) { status = "Select an instance."; return; }
                            done(engine.redo(id), "Redone."); });
  auto removeBtn = Button("Remove instance", [&]
                          { int id = selectedId(); if (id < 0) { status = "Select an instance."; return; }
                              done(engine.removeInstance(id), "Instance removed."); });

  auto strategyToggle = Toggle(&strategyNames, &strategySel);
  auto setStrategyBtn = Button("Apply strategy", [&]
                               { int id = selectedId(); if (id < 0 || strategySel < 0) { status = "Select an instance."; return; }
                                   done(engine.setStrategy(id, strategyNames[strategySel]), "Strategy changed."); });

  auto adapterToggle = Toggle(&adapterNames, &adapterSel);
  auto setAdapterBtn = Button("Use adapter", [&]
                              { int id = selectedId(); if (id < 0 || adapterSel < 0) { status = "Select an instance."; return; }
                                  done(engine.setAdapter(id, adapterNames[adapterSel]), "Adapter connected."); });
  auto recordIn = Input(&recordId, "record id");
  auto importBtn = Button("Import record", [&]
                          { int id = selectedId(); if (id < 0) { status = "Select an instance."; return; }
                              done(engine.importData(id, recordId), "Record imported into the payload."); });

  auto keyIn = Input(&key, "key");
  auto valIn = Input(&val, "value");
  auto payloadBtn = Button("Set payload", [&]
                           { int id = selectedId(); if (id < 0) { status = "Select an instance."; return; }
                               done(engine.setPayloadValue(id, key, val), "Payload updated."); });

  auto runControls = Container::Vertical({
      Container::Horizontal({bpMenu, instMenu, taskMenu}),
      Container::Horizontal({spawnBtn, startBtn, nextBtn, completeBtn, rejectBtn, cancelBtn}),
      Container::Horizontal({undoBtn, redoBtn, removeBtn}),
      Container::Horizontal({strategyToggle, setStrategyBtn}),
      Container::Horizontal({adapterToggle, setAdapterBtn, recordIn, importBtn}),
      Container::Horizontal({keyIn, valIn, payloadBtn}),
  });

  auto runView = Renderer(runControls, [&]
                          {
        const int id = selectedId();
        std::vector<TaskRow> rows;
        std::vector<std::string> history;
        std::map<std::string, std::string> payload;
        std::string info = "Spawn or select an instance.";

        if (id > 0) {
            rows = engine.getTaskRows(id);
            history = engine.getHistory(id);
            payload = engine.getPayloadMap(id);
            info = "Status: " + engine.getInstanceStatus(id) +
                   "   Strategy: " + engine.getStrategyName(id) +
                   "   Adapter: " + engine.getAdapterName(id);
        }

        taskLines.clear();
        for (const TaskRow &r : rows) {
            std::string line = std::string(r.depth * 2, ' ') + r.name + "  [" + r.state + "]";
            if (!r.assignee.empty())  line += "  -> " + r.assignee;
            if (!r.condition.empty()) line += "  if " + r.condition;
            taskLines.push_back(line);
        }
        if (taskSel >= (int)taskLines.size())
            taskSel = taskLines.empty() ? 0 : (int)taskLines.size() - 1;

        Elements payloadLines, historyLines;
        for (auto &kv : payload) payloadLines.push_back(text(kv.first + " = " + kv.second));
        size_t from = history.size() > 6 ? history.size() - 6 : 0;
        for (size_t i = from; i < history.size(); ++i) historyLines.push_back(text(history[i]));

        return vbox({
            hbox({
                vbox({text("Blueprints") | bold, bpMenu->Render()}) | border | flex,
                vbox({text("Instances") | bold, instMenu->Render()}) | border | flex,
                vbox({text("Tasks (select a row)") | bold, taskMenu->Render()}) | border | flex,
                vbox({
                    vbox({text("Payload") | bold, vbox(payloadLines)}) | border | flex,
                    vbox({text("History") | bold, vbox(historyLines)}) | border | flex,
                }) | flex,
            }) | flex,
            text(info) | dim,
            hbox({spawnBtn->Render(), startBtn->Render(), nextBtn->Render(),
                  completeBtn->Render(), rejectBtn->Render(), cancelBtn->Render()}),
            hbox({undoBtn->Render(), redoBtn->Render(), removeBtn->Render()}),
            hbox({text("Strategy: "), strategyToggle->Render(), text(" "), setStrategyBtn->Render()}),
            hbox({text("Adapter: "), adapterToggle->Render(), text(" "), setAdapterBtn->Render(),
                  text(" Record: "), recordIn->Render() | size(WIDTH, EQUAL, 10), text(" "),
                  importBtn->Render()}),
            text("  claims ids: C-102, C-103, C-104    hr ids: 1, 2, 3") | dim,
            hbox({text("Key: "), keyIn->Render() | size(WIDTH, EQUAL, 14),
                  text(" Value: "), valIn->Render() | size(WIDTH, EQUAL, 18), text(" "),
                  payloadBtn->Render()}),
        }); });

  // ---------------- Build page ----------------
  std::string bName, bVer = "1";
  std::string f1, f2, f3 = "1", f4;
  int kind = 0;
  std::vector<std::string> kinds = {"Department", "Employee", "Subprocess", "Task"};
  std::vector<std::string> bDepts, bSummary;
  std::vector<EmployeeSpec> bEmps;
  std::vector<SubprocessSpec> bSubs;
  std::vector<TaskSpec> bTasks;

  auto bNameIn = Input(&bName, "blueprint name");
  auto bVerIn = Input(&bVer, "1");
  auto kindToggle = Toggle(&kinds, &kind);
  auto f1In = Input(&f1, "name");
  auto f2In = Input(&f2, "parent / department");
  auto f3In = Input(&f3, "priority");
  auto f4In = Input(&f4, "condition key=value");

  auto addBtn = Button("Add item", [&]
                       {
        if (f1.empty()) { status = "The item needs a name."; return; }
        const std::string parent = f2.empty() ? "ROOT" : f2;
        switch (kind) {
            case 0:
                bDepts.push_back(f1);
                bSummary.push_back("Department: " + f1);
                break;
            case 1:
                bEmps.push_back({f1, f2});
                bSummary.push_back("Employee: " + f1 + (f2.empty() ? " (standalone)" : " -> " + f2));
                break;
            case 2:
                bSubs.push_back({f1, parent});
                bSummary.push_back("Subprocess: " + f1 + " (under " + parent + ")");
                break;
            case 3: {
                int prio = 1;
                try { prio = std::stoi(f3); } catch (...) {}
                bTasks.push_back({f1, parent, prio, f4});
                bSummary.push_back("Task: " + f1 + " (under " + parent + ", priority " + std::to_string(prio) + ")");
                break;
            }
        }
        status = "Added. Press 'Create blueprint' when you are done.";
        f1.clear(); });

  auto clearBuild = [&]
  {
    bDepts.clear();
    bEmps.clear();
    bSubs.clear();
    bTasks.clear();
    bSummary.clear();
  };

  auto createBtn = Button("Create blueprint", [&]
                          {
        int ver = 1;
        try { ver = std::stoi(bVer); } catch (...) {}
        bool ok = engine.assembleBlueprint(bName, ver, bDepts, bEmps, bSubs, bTasks);
        if (ok) { clearBuild(); bName.clear(); }
        done(ok, "Blueprint created. Select it on the Run page and Spawn."); });

  auto clearBtn = Button("Clear list", [&]
                         { clearBuild(); status = "Cleared."; });

  auto buildControls = Container::Vertical({
      Container::Horizontal({bNameIn, bVerIn}),
      kindToggle,
      Container::Horizontal({f1In, f2In, f3In, f4In}),
      Container::Horizontal({addBtn, createBtn, clearBtn}),
  });

  auto buildView = Renderer(buildControls, [&]
                            {
        Elements summary;
        size_t from = bSummary.size() > 12 ? bSummary.size() - 12 : 0;
        for (size_t i = from; i < bSummary.size(); ++i) summary.push_back(text(bSummary[i]));

        return vbox({
            text("New blueprint") | bold,
            hbox({text("Name: "), bNameIn->Render() | size(WIDTH, EQUAL, 28),
                  text("  Version: "), bVerIn->Render() | size(WIDTH, EQUAL, 5)}),
            hbox({text("Add: "), kindToggle->Render()}),
            hbox({f1In->Render() | size(WIDTH, EQUAL, 22), text(" "),
                  f2In->Render() | size(WIDTH, EQUAL, 22), text(" "),
                  f3In->Render() | size(WIDTH, EQUAL, 10), text(" "),
                  f4In->Render() | size(WIDTH, EQUAL, 24)}),
            text("  Parent/department empty = ROOT / standalone. Priority and condition only apply to tasks.") | dim,
            hbox({addBtn->Render(), text(" "), createBtn->Render(), text(" "), clearBtn->Render()}),
            separator(),
            text("Collected so far:") | bold,
            vbox(summary) | flex,
        }) | border | flex; });

  // ---------------- Report page ----------------
  std::vector<std::string> reportKinds = {"Selected instance", "Whole system"};
  int reportKind = 0, reportScroll = 0;

  auto instMenu2 = Menu(&instLabels, &instSel); // second view of the same selection
  auto reportToggle = Toggle(&reportKinds, &reportKind);
  auto scrollUp = Button("Scroll up", [&]
                         { reportScroll = std::max(0, reportScroll - 5); });
  auto scrollDown = Button("Scroll down", [&]
                           { reportScroll += 5; });
  auto exportBtn = Button("Export to file", [&]
                          { bool ok = engine.exportReport("tividy_report.txt");
                              status = ok ? "Report saved to tividy_report.txt" : engine.getLastError(); });

  auto reportControls = Container::Horizontal({
      instMenu2,
      Container::Vertical({reportToggle, Container::Horizontal({scrollUp, scrollDown, exportBtn})}),
  });

  auto reportView = Renderer(reportControls, [&]
                             {
        const int id = selectedId();
        std::vector<std::string> lines = (reportKind == 0 && id > 0)
                                             ? engine.generateReport(id)
                                             : engine.generateSystemReport();
        const int windowSize = 20;
        const int maxScroll = std::max(0, (int)lines.size() - windowSize);
        reportScroll = std::min(reportScroll, maxScroll);

        Elements shown;
        for (int i = reportScroll; i < (int)lines.size() && i < reportScroll + windowSize; ++i)
            shown.push_back(text(lines[i]));

        return hbox({
            vbox({text("Instances") | bold, instMenu2->Render()}) | border | size(WIDTH, EQUAL, 36),
            vbox({
                hbox({text("Show: "), reportToggle->Render()}),
                hbox({scrollUp->Render(), text(" "), scrollDown->Render(), text(" "), exportBtn->Render()}),
                separator(),
                vbox(shown) | flex,
                text("lines " + std::to_string(reportScroll + 1) + "+ of " + std::to_string(lines.size())) | dim,
            }) | border | flex,
        }) | flex; });

  // ---------------- Frame ----------------
  auto pageToggle = Toggle(&pageNames, &page);
  auto pages = Container::Tab({runView, buildView, reportView}, &page);
  auto quitBtn = Button("Quit", screen.ExitLoopClosure());
  auto root = Container::Vertical({Container::Horizontal({pageToggle, quitBtn}), pages});

  auto ui = Renderer(root, [&]
                     {
        Elements log;
        const auto &full = engine.getActivityLog();
        size_t start = full.size() > 7 ? full.size() - 7 : 0;
        for (size_t i = start; i < full.size(); ++i) log.push_back(text(full[i]));

        return vbox({
            hbox({text(" TIVIDY ") | bold | inverted, text(" "), pageToggle->Render(),
                  filler(), quitBtn->Render()}),
            separator(),
            pages->Render() | flex,
            separator(),
            text(" " + status) | bold,
            vbox({text("Activity log") | bold, vbox(log)}) | border,
        }); });

  screen.Loop(ui);
}
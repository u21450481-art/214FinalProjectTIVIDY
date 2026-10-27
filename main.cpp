#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
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

    // ---- existing state ----
    std::vector<std::string> bpNames, instLabels;
    std::vector<int> instIds;
    int bpSel = 0, instSel = 0;
    std::string key, val;

    auto refresh = [&]
    {
        bpNames = engine.getBlueprintNames();
        instIds = engine.getActiveInstanceIds();
        instLabels.clear();
        for (int id : instIds)
            instLabels.push_back("Instance #" + std::to_string(id));
    };
    refresh();

    // ---- new-blueprint state ----
    bool building = false;
    std::string bName, bVer = "1";
    std::string f1, f2, f3 = "1", f4; // name, parent/dept, priority, condition
    int kind = 0;
    std::vector<std::string> kinds = {"Department", "Employee", "Subprocess", "Task"};
    std::vector<std::string> buildSummary;
    std::string status = "Enter a name, then press Start.";

    auto bpMenu = Menu(&bpNames, &bpSel);
    auto instMenu = Menu(&instLabels, &instSel);
    auto keyIn = Input(&key, "key");
    auto valIn = Input(&val, "value");

    auto spawnBtn = Button("Spawn", [&]
                           {
    if (bpSel >= 0 && bpSel < (int)bpNames.size())
      engine.spawnInstance(bpNames[bpSel]);
    refresh(); });
    auto payloadBtn = Button("Set payload", [&]
                             {
    if (instSel >= 0 && instSel < (int)instIds.size() && !key.empty())
      engine.setPayloadValue(instIds[instSel], key, val); });
    auto quitBtn = Button("Quit", screen.ExitLoopClosure());

    // ---- new-blueprint components ----
    auto bNameIn = Input(&bName, "blueprint name");
    auto bVerIn = Input(&bVer, "1");
    auto kindToggle = Toggle(&kinds, &kind);
    auto f1In = Input(&f1, "name");
    auto f2In = Input(&f2, "parent / dept");
    auto f3In = Input(&f3, "prio");
    auto f4In = Input(&f4, "condition");

    auto startBtn = Button("Start", [&]
                           {
    if (bName.empty()) { status = "Give the blueprint a name first."; return; }
    int ver = 1;
    try { ver = std::stoi(bVer); } catch (...) {}
    engine.startCustomBlueprint(bName, ver);
    buildSummary.clear();
    building = true;
    status = "Building '" + bName + "'. Add items, then Finalize."; });

    auto addBtn = Button("Add", [&]
                         {
    if (!building) { status = "Press Start first."; return; }
    if (f1.empty()) { status = "Item needs a name."; return; }
    try {
      switch (kind) {
        case 0:
          engine.addCustomDepartment(f1);
          buildSummary.push_back("Dept: " + f1);
          break;
        case 1:
          engine.addCustomEmployee(f1, f2);
          buildSummary.push_back("Employee: " + f1 + (f2.empty() ? "" : " -> " + f2));
          break;
        case 2:
          engine.addCustomSubprocess(f1, f2.empty() ? "ROOT" : f2);
          buildSummary.push_back("Subprocess: " + f1 + " (under " + (f2.empty() ? "ROOT" : f2) + ")");
          break;
        case 3: {
          int prio = 1;
          try { prio = std::stoi(f3); } catch (...) {}
          engine.addCustomTask(f1, f2.empty() ? "ROOT" : f2, prio, f4);
          buildSummary.push_back("Task: " + f1 + " (under " + (f2.empty() ? "ROOT" : f2) + ", prio " + std::to_string(prio) + ")");
          break;
        }
      }
      status = "Added.";
      f1.clear();
    } catch (const std::exception &e) {
      status = std::string("Error: ") + e.what();
    } });

    auto finalBtn = Button("Finalize", [&]
                           {
    if (!building) { status = "Nothing to finalize. Press Start first."; return; }
    if (engine.finalizeCustomBlueprint()) {
      building = false;
      buildSummary.clear();
      status = "Blueprint registered. Select it on the left and Spawn.";
      refresh();
    } else {
      status = "Finalize failed (name already exists?).";
    } });

    auto layout = Container::Vertical({
        Container::Horizontal({bpMenu, instMenu}),
        Container::Horizontal({spawnBtn, quitBtn}),
        Container::Horizontal({keyIn, valIn, payloadBtn}),
        Container::Horizontal({bNameIn, bVerIn, startBtn}),
        kindToggle,
        Container::Horizontal({f1In, f2In, f3In, f4In, addBtn, finalBtn}),
    });

    auto ui = Renderer(layout, [&]
                       {
    Elements tree, payload, log, summary;

    if (instSel >= 0 && instSel < (int)instIds.size()) {
      int id = instIds[instSel];
      for (auto &l : engine.getInstanceTaskTreeStatus(id)) tree.push_back(text(l));
      for (auto &kv : engine.getPayloadMap(id))
        payload.push_back(text(kv.first + " = " + kv.second));
    }

    const auto &full = engine.getActivityLog();
    size_t start = full.size() > 8 ? full.size() - 8 : 0;
    for (size_t i = start; i < full.size(); ++i) log.push_back(text(full[i]));

    size_t s0 = buildSummary.size() > 6 ? buildSummary.size() - 6 : 0;
    for (size_t i = s0; i < buildSummary.size(); ++i) summary.push_back(text(buildSummary[i]));

    return vbox({
        hbox({
            vbox({text("Blueprints") | bold, bpMenu->Render()}) | border | flex,
            vbox({text("Instances") | bold, instMenu->Render()}) | border | flex,
            vbox({text("Task tree (clone)") | bold, vbox(tree)}) | border | flex,
            vbox({text("Payload") | bold, vbox(payload)}) | border | flex,
        }) | flex,
        hbox({spawnBtn->Render(), quitBtn->Render()}),
        hbox({text("Key: "), keyIn->Render() | size(WIDTH, EQUAL, 16),
              text(" Value: "), valIn->Render() | size(WIDTH, EQUAL, 20),
              payloadBtn->Render()}),
        vbox({
            text("New blueprint") | bold,
            hbox({text("Name: "), bNameIn->Render() | size(WIDTH, EQUAL, 24),
                  text(" Version: "), bVerIn->Render() | size(WIDTH, EQUAL, 4),
                  text(" "), startBtn->Render()}),
            hbox({text("Add: "), kindToggle->Render()}),
            hbox({text("Name: "), f1In->Render() | size(WIDTH, EQUAL, 18),
                  text(" Parent/Dept: "), f2In->Render() | size(WIDTH, EQUAL, 18),
                  text(" Prio: "), f3In->Render() | size(WIDTH, EQUAL, 4),
                  text(" Cond: "), f4In->Render() | size(WIDTH, EQUAL, 18),
                  text(" "), addBtn->Render(), text(" "), finalBtn->Render()}),
            text(status) | dim,
            vbox(summary),
        }) | border,
        vbox({text("Activity log") | bold, vbox(log)}) | border,
    }); });

    screen.Loop(ui);
}
/**
 * @file test_engine.cpp
 * @brief GoogleTest suite for the Workflow Engine:Builder and facade as well as command and adapter
 * This file tests the four core design patterns used in your engine:
 * 1. Builder: assembling
 * 2. Adapter: importing legacy/external system data
 * 3. Command: executing tasks with Undo/Redo support
 * 4. Facade:
 */

#include <gtest/gtest.h>
#include "WorkflowEngine.h"

//BUILDER
//verifies that the builder can create and register a cutoms workflow
TEST(WorkflowEngineTest, TestBuilderAssemblesBlueprint) {
    WorkflowEngine engine;

    //call facade which uses builder
    bool isAssembled = engine.assembleBlueprint(
        "Test_Workflow", //name of workflow
        1, //version
        {"IT Department"},  //departments
        {{"Alice", "IT Department"}}, ///employees and thei roles
        {{"ReviewStage", "ROOT"}}, //subprocesses
        {{"Code Review", "ReviewStage", 1, ""}} //tasks
    );

    EXPECT_TRUE(isAssembled); //asserts that assembly succeeded

    //verifiying the workflow actually exists
    auto blueprints = engine.getBlueprintNames();
    bool found = false;
    for (const std::string& name : blueprints) {
        if (name == "Test_Workflow") {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

//ADAPTER
// verifies an external daat adapter can get record data from a legacy source and populate workflow payload
TEST(WorkflowEngineTest, TestAdapterImportsData) {
    WorkflowEngine engine;

    //setup a simple workflow
    bool assembled = engine.assembleBlueprint(
        "Insurance_Claim", 1,
        {"Claims"},
        {{"Bob", "Claims"}},
        {{"Assessment", "ROOT"}},
        {{"Assess Claim", "Assessment", 1, "claim_amount > 5000"}}
    );
    ASSERT_TRUE(assembled);

    std::string strategy = engine.getStrategyNames()[0];
    int instanceId = engine.spawnInstance("Insurance_Claim", strategy);
    
    //print the exact engine error if it fails to spawn
    ASSERT_GE(instanceId, 0) << "Spawn failed: " << engine.getLastError();

    bool adapterSet = engine.setAdapter(instanceId, "claims"); // Note: adapter name is "claims" in makeAdapter
    EXPECT_TRUE(adapterSet) << "Adapter error: " << engine.getLastError();

    //import external record data using adapter
    bool dataImported = engine.importData(instanceId, "C-102");
    EXPECT_TRUE(dataImported) << "Import error: " << engine.getLastError();

    //is the data payload empty after importing
    auto payload = engine.getPayloadMap(instanceId);
    EXPECT_FALSE(payload.empty());
}

//COMMAND
//verifies tha workflow executing steps are wrapped in commands,toallow undoing and redoing
TEST(WorkflowEngineTest, TestCommandUndoRedo) {
    WorkflowEngine engine;

    bool assembled = engine.assembleBlueprint(
        "Simple_Flow", 1,
        {"Operations"},
        {{"Charlie", "Operations"}},
        {},
        {{"Task A", "ROOT", 1, ""}}
    );
    ASSERT_TRUE(assembled);

    std::string strategy = engine.getStrategyNames()[0];
    int instanceId = engine.spawnInstance("Simple_Flow", strategy);
    ASSERT_GE(instanceId, 0) << "Spawn failed: " << engine.getLastError();

    EXPECT_TRUE(engine.startInstance(instanceId)) << engine.getLastError();
    
    //complete task at index 1 (Index 0 is the root group, Index 1 is "Task A")
    EXPECT_TRUE(engine.completeTask(instanceId, 1)) << engine.getLastError();

    //test Undo: should successfully revert the completion command
    bool undone = engine.undo(instanceId);
    EXPECT_TRUE(undone) << engine.getLastError();

    //test Redo
    bool redone = engine.redo(instanceId);
    EXPECT_TRUE(redone) << engine.getLastError();
}

//FACADE
//verifies that the facade can spawn multiple instances of the same workflow and that they are isolated from each other
TEST(WorkflowEngineTest, TestFacadeInstanceIsolation) {
    WorkflowEngine engine;

    bool assembled = engine.assembleBlueprint(
        "Shared_Blueprint", 1,
        {"Dept"},
        {{"Emp", "Dept"}},
        {},
        {{"Task 1", "ROOT", 1, ""}}
    );
    ASSERT_TRUE(assembled);

    //spawn two separate instances from the same blueprint
    std::string strategy = engine.getStrategyNames()[0];
    int id1 = engine.spawnInstance("Shared_Blueprint", strategy);
    int id2 = engine.spawnInstance("Shared_Blueprint", strategy);

    ASSERT_GE(id1, 0) << engine.getLastError();
    ASSERT_GE(id2, 0) << engine.getLastError();

    //both instances should have unique IDs
    EXPECT_NE(id1, id2);
    EXPECT_EQ(engine.getInstanceStatus(id1), "Not started");
    EXPECT_EQ(engine.getInstanceStatus(id2), "Not started");
}

//ERROR HANDLING
//verifies that the engine safely catches invalid operations
TEST(WorkflowEngineTest, TestInvalidOperationsHandledSafely) {
    WorkflowEngine engine;

    //start an instance ID that doesn't exist should return false
    EXPECT_FALSE(engine.startInstance(9999));

    //complete a task on an invalid instance should fail gracefully
    EXPECT_FALSE(engine.completeTask(9999, 0));

    //error message should be recorded
    EXPECT_FALSE(engine.getLastError().empty());
}

//MAIN
int main(int argc, char **argv) {
    //initialize GoogleTest framework
    ::testing::InitGoogleTest(&argc, argv);
    //execute all unit tests and return the result
    return RUN_ALL_TESTS();
}
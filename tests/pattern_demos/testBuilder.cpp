/**
 * @file main.cpp
 * @brief Comprehensive test driver for Builder, Director, WorkflowDefinition,
 *        WorkflowInstance, and DataPayload.
 */

#include <cassert>
#include <memory>
#include "Log.h"

#include "CustomDefinitionBuilder.h"
#include "Director.h"
#include "WorkflowDefinition.h"
#include "WorkflowInstance.h"
#include "DataPayload.h"

// Uncomment these if you want to inspect the Composite pointers directly in main:
#include "WorkflowComponent.h"
#include "Subprocess.h"
#include "Task.h"
#include "Participant.h"

void printHeader(const std::string &title)
{
    TLOG("\n============================================================");
    TLOG("  " << title);
    TLOG("============================================================");
}

int main()
{

    // test standard Director, Builder

    printHeader("TEST 1: GoF Director + CustomDefinitionBuilder Sequence");
    CustomDefinitionBuilder builder;
    Director director(&builder);

    director.setBuilder(&builder);
    director.Construct(); // calls reset() -> buildParticipants() -> buildTaskTree()

    std::unique_ptr<WorkflowDefinition> defaultDef(builder.getResult());
    assert(defaultDef != nullptr);
    TLOG("[PASS] Director constructed definition: '" << defaultDef->getName()
         << "' (version" << defaultDef->getVersion() << ")");

    // test ownership handover (calling getResult() again right away should return nullptr)
    assert(builder.getResult() == nullptr);
    TLOG("[PASS] Builder ownership transfer verified (internal pointer reset to nullptr).");

    // industry 1 — Retail "Return_Item"

    printHeader("TEST 2: Building Industry 1 (Retail_Return_Item)");
    builder.reset("Retail_Return_Item", 1);

    // build tree employees
    builder.addDepartment("Customer Service");
    builder.addEmployee("Lerato (Returns Clerk)", "Customer Service", "Returns Clerk", 8);
    builder.addDepartment("Warehouse & Quality");
    builder.addEmployee("Sipho (Quality Inspector)", "Warehouse & Quality", "Quality Inspector", 11);
    builder.addEmployee("Thabo (Store Manager)", "", "Store Manager", 15); // alone employee

    // build task tree
    builder.addTask("Scan Receipt & Log Return Reason", "ROOT", 1);

    builder.addSubprocess("Physical_Inspection_Phase", "ROOT");
    builder.addTask("Inspect Returned Garment for Damage", "Physical_Inspection_Phase", 1);
    builder.addTask("Restock Shelf Inventory", "Physical_Inspection_Phase", 2, "itemCondition=Intact");

    builder.addSubprocess("Financial_Settlement_Phase", "ROOT");
    builder.addTask("Approve Refund or Reject Claim", "Financial_Settlement_Phase", 1);
    builder.addTask("Send Refund Confirmation Email", "Financial_Settlement_Phase", 2);

    std::unique_ptr<WorkflowDefinition> retailDef(builder.getResult());
    assert(retailDef != nullptr);
    assert(retailDef->getName() == "Retail_Return_Item");
    assert(retailDef->getVersion() == 1);

    // WorkflowDefinition getters/setters
    retailDef->setVersion(2);
    assert(retailDef->getVersion() == 2);
    retailDef->setName("Retail_Return_Item_Updated");
    assert(retailDef->getName() == "Retail_Return_Item_Updated");
    retailDef->setName("Retail_Return_Item"); // restore name

    TLOG("[PASS] Retail Definition built. Registered top-level participants: "
         << retailDef->getParticipants().size());

    // industry 2 — Banking "Bank_Process_Loan" Using Same Builder

    printHeader("TEST 3: Building Industry 2 (Bank_Process_Loan)");
    builder.reset("Bank_Process_Loan", 1);

    builder.addDepartment("Credit Underwriting");
    builder.addEmployee("Naledi (Credit Analyst)", "Credit Underwriting", "Credit Analyst", 7);
    builder.addDepartment("Risk & Compliance");
    builder.addEmployee("Johann (Risk Officer)", "Risk & Compliance", "Risk Officer", 10);

    builder.addTask("Verify FICA Documents & Income", "ROOT", 1);
    builder.addSubprocess("Credit_Assessment_Phase", "ROOT");
    builder.addTask("Run Credit Bureau Check", "Credit_Assessment_Phase", 1);
    builder.addTask("Calculate Debt-to-Income Ratio", "Credit_Assessment_Phase", 2);

    builder.addSubprocess("Disbursement_Phase", "ROOT");
    builder.addTask("Committee Sign-Off on Loan", "Disbursement_Phase", 1, "creditStatus=Approved");
    builder.addTask("Execute EFT Payout", "Disbursement_Phase", 2);

    WorkflowDefinition *bankDef = builder.getResult();
    assert(bankDef != nullptr);
    assert(bankDef->getName() == "Bank_Process_Loan");
    TLOG("[PASS] Banking Definition built with the exact same builder class!");

    // TEST 4: Spawning Isolated WorkflowInstances (Prototype / instantiate)

    printHeader("TEST 4: Spawning Concurrent WorkflowInstances & Testing Isolation");
    std::unique_ptr<WorkflowInstance> retailInst1 = retailDef->instantiate();
    std::unique_ptr<WorkflowInstance> retailInst2 = retailDef->instantiate();
    std::unique_ptr<WorkflowInstance> bankInst1 = bankDef->instantiate();

    assert(retailInst1->getId() == 1);
    assert(retailInst2->getId() == 2);
    assert(bankInst1->getId() == 1);
    assert(retailInst1->getDefinitionName() == "Retail_Return_Item");
    assert(retailInst1->getDefinitionVersion() == 2);

    // WorkflowComponent::clone() test deep copy pointer isolation
    if (retailDef->getRoot() != nullptr && retailInst1->getRoot() != nullptr)
    {
        assert(retailInst1->getRoot() != retailDef->getRoot());
        assert(retailInst1->getRoot() != retailInst2->getRoot());
        TLOG("[PASS] Prototype Deep Copy Verified: Instance roots have distinct memory addresses!");
    }
    else
    {
        TLOG("[INFO] Root pointer is currently nullptr");
    }

    // TEST 5: DataPayload CRUD Operations & Runtime Isolation Across Instances

    printHeader("TEST 5: Testing DataPayload Operations & Instance Isolation");
    DataPayload &payload1 = retailInst1->getPayload();
    DataPayload &payload2 = retailInst2->getPayload();

    // Populate Instance #1 Payload
    payload1.set("customerName", "Jane Doe");
    payload1.set("itemCondition", "Intact");
    payload1.set("refundAmount", "R450.00");
    payload1.set("tempFlag", "DeleteMe");

    // Populate Instance #2 Payload with completely different data
    payload2.set("customerName", "John Smith");
    payload2.set("itemCondition", "Damaged");

    assert(payload1.has("customerName") == true);
    assert(payload1.get("customerName") == "Jane Doe");
    assert(payload2.get("customerName") == "John Smith");
    assert(payload1.get("nonExistentKey") == "");

    // Test remove()
    assert(payload1.remove("tempFlag") == true);
    assert(payload1.has("tempFlag") == false);
    assert(payload1.remove("tempFlag") == false); // already removed

    TLOG("\n--- Retail Instance #1 Payload ---");
    payload1.printPayload();

    TLOG("\n--- Retail Instance #2 Payload ---");
    payload2.printPayload();

    // DataPayload Copy Constructor, clear()
    DataPayload copiedPayload(payload1);
    assert(copiedPayload.get("refundAmount") == "R450.00");
    copiedPayload.clear();
    assert(copiedPayload.getAll().empty() == true);
    assert(payload1.has("refundAmount") == true); // og must be untouched
    TLOG("[PASS] DataPayload CRUD, copy constructor, and instance isolation verified!");

    // TEST 6: WorkflowInstance Lifecycle Methods (start / stop)

    printHeader("TEST 6: Testing WorkflowInstance start() and stop()");
    retailInst1->start();
    retailInst1->stop();

    delete bankDef;

    printHeader("ALL BUILDER, PROTOTYPE & PAYLOAD TESTS PASSED SUCCESSFULLY!");
    return 0;
}
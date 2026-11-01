/**
 * @file testCoR.cpp
 * @brief Standalone unit test driver for the Chain of Responsibility (CoR) approval workflow.
 */

#include <cassert>
#include <iostream>
#include <string>
#include <algorithm>

void printHeader(const std::string &title)
{
    std::cout << "\n============================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "============================================================\n";
}

enum AuthorityLevel { LEVEL_1 = 1, LEVEL_2 = 2, LEVEL_3 = 3, LEVEL_EXCEEDED = 4 };

struct ApprovalRequest {
    std::string taskName;
    int level;
    ApprovalRequest(std::string name, int lvl) : taskName(name), level(lvl) {}
};

class Employee {
private:
    std::string name;
    std::string role;
public:
    Employee(std::string n, std::string r, int) : name(n), role(r) {}
    std::string getName() const { return name; }
};

class ApprovalHandler {
protected:
    ApprovalHandler* successor;
    int maxLevel;
    std::string approverName;
    Employee* approverEmp;
    std::string* logRef;

public:
    ApprovalHandler(int limit, const std::string& name, Employee* emp, std::string* log)
        : successor(nullptr), maxLevel(limit), approverName(name), approverEmp(emp), logRef(log) {}

    void setNext(ApprovalHandler* nextHandler) {
        successor = nextHandler;
    }

    virtual void handleRequest(ApprovalRequest& request) {
        if (request.level <= maxLevel) {
            if (logRef) {
                *logRef = "[CoR] (request was handled by " + approverName + " (level " + std::to_string(maxLevel) + "))";
            }
        } 
        else if (successor != nullptr) {
            successor->handleRequest(request);
        } 
        else {
            if (logRef) {
                *logRef = "[CoR] (request exceeded maximum authority limits)";
            }
        }
    }
    virtual ~ApprovalHandler() = default;
};

int main()
{
    printHeader("SETUP: Initializing Employees and CoR Chain Nodes");

    Employee mgr("Charlie", "Manager", 5);
    Employee dir("Diana", "Director", 10);
    Employee ceo("Vimbai", "Chief Executive Officer", 20);

    // TEST 1: Level 1 Request (Handled by Manager - Charlie)
    {
        printHeader("TEST 1: Request Level 1 (Manager Authority)");
        std::string logResult = "";
        
        ApprovalHandler managerNode(1, "Charlie", &mgr, &logResult);
        ApprovalHandler directorNode(2, "Diana", &dir, &logResult);
        ApprovalHandler ceoNode(3, "Vimbai", &ceo, &logResult);

        managerNode.setNext(&directorNode);
        directorNode.setNext(&ceoNode);

        ApprovalRequest req("Low-value Payout", LEVEL_1);
        managerNode.handleRequest(req);

        std::cout << "[LOG OUTPUT] " << logResult << "\n";
        assert(logResult == "[CoR] (request was handled by Charlie (level 1))");
        std::cout << "[PASS] Level 1 request successfully handled by Charlie!\n";
    }

    // TEST 2: Level 2 Request (Handled by Director - Diana)
    {
        printHeader("TEST 2: Request Level 2 (Director Authority)");
        std::string logResult = "";

        ApprovalHandler managerNode(1, "Charlie", &mgr, &logResult);
        ApprovalHandler directorNode(2, "Diana", &dir, &logResult);
        ApprovalHandler ceoNode(3, "Vimbai", &ceo, &logResult);

        managerNode.setNext(&directorNode);
        directorNode.setNext(&ceoNode);

        ApprovalRequest req("Insurance Claim Payout", LEVEL_2);
        managerNode.handleRequest(req);

        std::cout << "[LOG OUTPUT] " << logResult << "\n";
        assert(logResult == "[CoR] (request was handled by Diana (level 2))");
        std::cout << "[PASS] Level 2 request successfully handled by Diana!\n";
    }

    // TEST 3: Level 3 Request (Handled by CEO - Vimbai)
    {
        printHeader("TEST 3: Request Level 3 (CEO Authority)");
        std::string logResult = "";

        ApprovalHandler managerNode(1, "Charlie", &mgr, &logResult);
        ApprovalHandler directorNode(2, "Diana", &dir, &logResult);
        ApprovalHandler ceoNode(3, "Vimbai", &ceo, &logResult);

        managerNode.setNext(&directorNode);
        directorNode.setNext(&ceoNode);

        ApprovalRequest req("Enterprise Claim Payout", LEVEL_3);
        managerNode.handleRequest(req);

        std::cout << "[LOG OUTPUT] " << logResult << "\n";
        assert(logResult == "[CoR] (request was handled by Vimbai (level 3))");
        std::cout << "[PASS] Level 3 request successfully handled by Vimbai!\n";
    }

    // TEST 4: Exceeding Maximum Authority Limits
    {
        printHeader("TEST 4: Request Exceeding Max Authority Limits");
        std::string logResult = "";

        ApprovalHandler managerNode(1, "Charlie", &mgr, &logResult);
        ApprovalHandler directorNode(2, "Diana", &dir, &logResult);
        ApprovalHandler ceoNode(3, "Vimbai", &ceo, &logResult);

        managerNode.setNext(&directorNode);
        directorNode.setNext(&ceoNode);

        ApprovalRequest req("Extremely High-value Payout", LEVEL_EXCEEDED);
        managerNode.handleRequest(req);

        std::cout << "[LOG OUTPUT] " << logResult << "\n";
        assert(logResult == "[CoR] (request exceeded maximum authority limits)");
        std::cout << "[PASS] Exceeding authority limit correctly caught and logged!\n";
    }

    printHeader("ALL CHAIN OF RESPONSIBILITY TESTS PASSED SUCCESSFULLY!");
    return 0;
}
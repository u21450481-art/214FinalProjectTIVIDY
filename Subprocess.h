#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#include <memory>
#include <vector>

#include "WorkflowComponent.h"
#include "Task.h"

using namespace std;

class Subprocess : public WorkflowComponent
{
    vector<WorkflowComponent *> children;
    std::string name; // added this to integrate with builder

public:
    Subprocess(std::string processName = "Unnamed_Subprocess"); // added name
    ~Subprocess();
    void execute();
    void add(WorkflowComponent *c) override;
    void remove(WorkflowComponent *c) override;
    vector<WorkflowComponent *> getChildren() override;
    bool allChildrenComplete();

    // PROTOTYPE
    virtual WorkflowComponent *clone() const override;
};

#endif
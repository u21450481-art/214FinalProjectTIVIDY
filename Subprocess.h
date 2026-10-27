#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#include <memory>
#include <vector>

#include "WorkflowComponent.h"
#include "Task.h"

using namespace std;

class Subprocess: public WorkflowComponent {
    vector<WorkflowComponent*> children;

    public:
    Subprocess();
    ~Subprocess();
    void execute();
    void add(WorkflowComponent* c) override;
    void remove(WorkflowComponent* c) override;
    vector<WorkflowComponent*> getChildren() override;
    bool allChildrenComplete();
    
};




#endif
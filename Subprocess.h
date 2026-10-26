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
    Subprocess(WorkflowComponent*);
    ~Subprocess();
    void execute();
    void add(WorkflowComponent* c);
    void remove(WorkflowComponent* c);
    vector<WorkflowComponent*> getChildren();
    bool allChildrenComplete();
    
};




#endif
#ifndef SUBPROCESS_H
#define SUBPROCESS_H


#include <vector>

#include "WorkflowComponent.h"

class Subprocess: public WorkflowComponent {
    vector<unique_ptr<WorkflowComponent>> children;

    public:
    void execute();
    void add(WorkflowComponent c);
    void remove(WorkflowComponent c);
    WorkflowComponent* getChild(int index);
    void allChilderComplete();
    
};




#endif
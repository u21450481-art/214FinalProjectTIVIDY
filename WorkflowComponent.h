#ifndef WORKFLOWCOMPONENT_H
#define WORKFLOWCOMPONENT_H

#include <memory>
#include <vector>

#include "WorkflowIterator.h"
#include "Stage.h"

class WorkflowOrchestrator;

class WorkflowComponent {
    WorkflowOrchestrator* mediator;
    Stage*  state;

    public:
    WorkflowComponent();
    virtual ~WorkflowComponent();

    virtual void execute() = 0;
    virtual void add(WorkflowComponent* c)  = 0;
    virtual void remove(WorkflowComponent* c)  = 0;
    virtual vector<WorkflowComponent*> getChildren()  = 0;
    
    Stage*  getState();
    void setState(Stage* s);
    WorkflowIterator* createIterator();
    void start();
    void complete();
    void fail();
    void cancel();
    void setMediator(WorkflowOrchestrator* mediator);
    
};


#endif
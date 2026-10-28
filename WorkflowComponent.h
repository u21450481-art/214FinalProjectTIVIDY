#ifndef WORKFLOWCOMPONENT_H
#define WORKFLOWCOMPONENT_H

#include <string>
#include <vector>
#include <memory>

#include "WorkflowIterator.h"
#include "Stage.h"

using namespace std;

class WorkflowOrchestrator;

class WorkflowComponent {
    WorkflowOrchestrator* mediator;
    Stage*  state; //a shared state instance, not owned

    public:
    WorkflowComponent();
    virtual ~WorkflowComponent();

    virtual void execute() = 0;
    virtual std::string getName() = 0;
    virtual void add(WorkflowComponent* c)  {};
    virtual void remove(WorkflowComponent* c)  {};
    virtual vector<WorkflowComponent*> getChildren() {return {};};
    
    Stage*  getState();
    void setState(Stage* s);
    WorkflowIterator* createIterator();
    void start();
    void complete();
    void reject();
    void cancel();
    void setMediator(WorkflowOrchestrator* mediator);
    
};


#endif

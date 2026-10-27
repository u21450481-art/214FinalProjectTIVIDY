#ifndef WORKFLOWCOMPONENT_H
#define WORKFLOWCOMPONENT_H

#include <memory>
#include <vector>

#include "WorkflowIterator.h"
#include "Stage.h"

using namespace std;

class WorkflowOrchestrator;

class WorkflowComponent
{
    WorkflowOrchestrator *mediator;
    Stage *state;

public:
    WorkflowComponent();
    virtual ~WorkflowComponent();

    virtual void execute() = 0;
    virtual void add(WorkflowComponent *c) {};
    virtual void remove(WorkflowComponent *c) {};
    virtual vector<WorkflowComponent *> getChildren() { return {}; };

    Stage *getState();
    void setState(Stage *s);
    WorkflowIterator *createIterator(); // this is a factory method
    void start();
    void complete();
    void fail();
    void cancel();
    void setMediator(WorkflowOrchestrator *mediator);

    // ADDED THIS FOR PROTOTYPE PATTERN
    virtual WorkflowComponent *clone() const = 0;
};
#endif
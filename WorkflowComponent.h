#ifndef WORKFLOW_COMPONENT
#define WORKFLOW_COMPONENT

class WorkflowOrchestrator{};

class Stage{};

class WorkflowIterator{};

class WorkflowComponent {
    WorkflowOrchestrator* mediator;
    Stage state;

    public:
    virtual void execute() = 0;
    virtual void add(WorkflowComponent c)  = 0;
    virtual void remove(WorkflowComponent c)  = 0;
    virtual WorkflowComponent* getChild(int index)  = 0;
    virtual Stage getState()  = 0;
    virtual void setState(Stage s)  = 0;
    virtual WorkflowIterator* createIterator()  = 0;
    virtual void start()  = 0;
    virtual void complete()  = 0;
    virtual void fail()  = 0;
    virtual void cancel()  = 0;
    virtual void setMediator(WorkflowOrchestrator* mediator)  = 0;
    virtual void clone()  = 0;
    

};


#endif
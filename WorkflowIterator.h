#ifndef WORKFLOWITERATOR_H
#define WORKFLOWITERATOR_H

class WorkflowComponent;

class WorkflowIterator{
    
public:
    virtual WorkflowComponent* first() = 0;
    virtual WorkflowComponent* next() = 0;
    virtual bool isDone() = 0;
    virtual WorkflowComponent* currentItem() = 0;
};

#endif
#ifndef WORKFLOWITERATOR_H
#define WORKFLOWITERATOR_H

class WorkflowComponent;

class WorkflowIterator{
    
public:
    virtual ~WorkflowIterator() {} // so deleting through the base pointer cleans up the real iterator
    virtual WorkflowComponent* first() = 0;
    virtual WorkflowComponent* next() = 0;
    virtual bool isDone() = 0;
    virtual WorkflowComponent* currentItem() = 0;
};

#endif
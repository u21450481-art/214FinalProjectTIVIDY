#ifndef DEPTHFIRSTITERATOR_H
#define DEPTHFIRSTITERATOR_H

#include <vector>
using namespace std;

#include "WorkflowComponent.h"
#include "WorkflowIterator.h"

class depthFirstIterator: public WorkflowIterator{
    WorkflowComponent* root;
    vector<WorkflowComponent*> pending;

public:
    depthFirstIterator(WorkflowComponent*);
    ~depthFirstIterator();
    WorkflowComponent* first() override;
    WorkflowComponent* next() override;
    bool isDone()override;
    WorkflowComponent* currentItem()override;
};
#endif
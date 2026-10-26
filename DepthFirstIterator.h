#ifndef DEPTHFIRSTITERATOR_H
#define DEPTHFIRSTITERATOR_H

#include <vector>
using namespace std;

#include "WorkflowComponent.h"
#include "WorkflowIterator.h"

class depthFirstIterator: public WorkflowIterator{
    WorkflowComponent* root;
    vector<WorkflowComponent*> list;

public:
    depthFirstIterator(WorkflowComponent*);
    WorkflowComponent* first();
    WorkflowComponent* next();
    bool isDone();
    WorkflowComponent* currentItem();
};
#endif
#ifndef COMPLETEDTASKITERATOR_H
#define COMPLETEDTASKITERATOR_H

#include "WorkflowIterator.h"
#include "Subprocess.h"
#include "WorkflowComponent.h"

#include <vector>

class CompletedTaskIterator : public WorkflowIterator
{
    WorkflowComponent *root;
    std::vector<WorkflowComponent *> matches;
    size_t index = 0;

public:
    explicit CompletedTaskIterator(WorkflowComponent *r);
    ~CompletedTaskIterator();
    WorkflowComponent *first() override;
    WorkflowComponent *next() override;
    bool isDone() override;
    WorkflowComponent *currentItem() override;
};
#endif
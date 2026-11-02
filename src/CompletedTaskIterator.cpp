#include "CompletedTaskIterator.h"
#include "DepthFirstIterator.h"
#include "Task.h"

CompletedTaskIterator::CompletedTaskIterator(WorkflowComponent *r) : root(r), index(0)
{
    first();
}

CompletedTaskIterator::~CompletedTaskIterator() {}

WorkflowComponent *CompletedTaskIterator::first()
{
    matches.clear();
    index = 0;
    // reuse the depth first walk and keep only the tasks that are complete
    depthFirstIterator it(root);
    for (; !it.isDone(); it.next())
    {
        WorkflowComponent *c = it.currentItem();
        if (dynamic_cast<Task *>(c) && c->getState()->getName() == "Complete")
            matches.push_back(c);
    }
    return currentItem();
}

WorkflowComponent *CompletedTaskIterator::next()
{
    if (index < matches.size())
        ++index;
    return currentItem();
}

bool CompletedTaskIterator::isDone() { return index >= matches.size(); }

WorkflowComponent *CompletedTaskIterator::currentItem()
{
    return isDone() ? nullptr : matches[index];
}
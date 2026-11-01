#include "CompletedTaskIterator.h"
#include "DepthFirstIterator.h"

CompletedTaskIterator::CompletedTaskIterator(WorkflowComponent* c = nullptr) :root(c){}

CompletedTaskIterator::~CompletedTaskIterator(){}

WorkflowComponent* CompletedTaskIterator::first(){
   matches.clear();
        index = 0;
        depthFirstIterator it(root);
        while(!it.isDone()) {
            WorkflowComponent* c = it.currentItem();
            if (dynamic_cast<Task*>(c) && c->getState()->getName() == "Complete")
                matches.push_back(c);
            it.next();
        }
        return currentItem();
}

WorkflowComponent* CompletedTaskIterator::next(){
     if (index < matches.size()) ++index;
        return currentItem();
}

bool CompletedTaskIterator::isDone()  { return index >= matches.size(); }

WorkflowComponent* CompletedTaskIterator::currentItem()  {
     return isDone() ? nullptr : matches[index];
    }
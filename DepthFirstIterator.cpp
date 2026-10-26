#include "DepthFirstIterator.h"

depthFirstIterator::depthFirstIterator(WorkflowComponent* c = nullptr)
:root(c){
    first();
}

depthFirstIterator::~depthFirstIterator(){}

WorkflowComponent* depthFirstIterator::first(){
   pending.clear();
    if (root) pending.push_back(root);
    return currentItem();
}

WorkflowComponent* depthFirstIterator::next(){
    if (pending.empty()) return nullptr;
   vector< WorkflowComponent*> c = pending.back()->getChildren();
   pending.pop_back();
    for(auto i = c.rbegin(); i!=c.rend(); i++){
        pending.push_back(*i);
    }
return currentItem();
}

bool depthFirstIterator::isDone(){
    return pending.empty();
}

WorkflowComponent* depthFirstIterator::currentItem(){
    return  pending.empty() ? nullptr : pending.back();
}


#include "DepthFirstIterator.h"

depthFirstIterator::depthFirstIterator(WorkflowComponent* c = nullptr)
:root(c){
    first();
}

WorkflowComponent* depthFirstIterator::first(){
   list.clear();
    if (root) list.push_back(root);
    return currentItem();
}

WorkflowComponent* depthFirstIterator::next(){
    if (list.empty()) return nullptr;
   vector< WorkflowComponent*> c = list.front()->getChildren();
   list.pop_back();
    for(auto i = c.rbegin(); i!=c.rend(); i++){
        list.push_back(*i);
    }
return currentItem();
}

bool depthFirstIterator::isDone(){
    return list.empty();
}

WorkflowComponent* depthFirstIterator::currentItem(){
    return  list.empty() ? nullptr : list.back();
}


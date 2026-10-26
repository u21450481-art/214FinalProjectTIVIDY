#include "Subprocess.h"
#include <algorithm>

Subprocess::Subprocess(WorkflowComponent* c){
    children = {c}; 
}

Subprocess::~Subprocess() { 
    for (auto* c : children) delete c;
}

void Subprocess::add(WorkflowComponent* c){
    if (c)
    children.push_back(c);
}

void Subprocess::remove(WorkflowComponent* c){
 children.erase(std::remove(children.begin(), children.end(), c), children.end());
}

vector<WorkflowComponent*> Subprocess::getChildren(){
    return children;
}

void Subprocess::execute(){
    for (auto* c : children) c->execute();
}

bool Subprocess::allChildrenComplete(){
    for (auto* c : children)
        if (c->getState()->getName() != "Complete") return false;
    return true;
}


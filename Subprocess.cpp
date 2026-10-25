#include "Subprocess.h"

Subprocess::Subprocess(WorkflowComponent* c){
    children = {c}; 
}

void Subprocess::add(WorkflowComponent* c){
    children.push_back(c);
}

void Subprocess::remove(WorkflowComponent* c){

}

WorkflowComponent* Subprocess::getChild(int index){
    return children[index];
}


void Subprocess::execute(){
    this->start();
}

void Subprocess::allChildrenComplete(){
    
}


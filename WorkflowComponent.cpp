#include "WorkflowComponent.h"
#include "DepthFirstIterator.h"
#include "ConcreteStages.h"


WorkflowComponent::WorkflowComponent() :
   mediator(nullptr), state(new NotStarted()) {}

WorkflowComponent::~WorkflowComponent() { 
    delete state; 
}

void WorkflowComponent::setState(Stage* s){
    delete state; 
    state = s; 
}

Stage* WorkflowComponent::getState(){
    return state;
}

void WorkflowComponent::setMediator(WorkflowOrchestrator* m){
    mediator = m;
}

void WorkflowComponent::start(){
    state->start(this);
}

void WorkflowComponent::complete(){
    state->complete(this);
}

void WorkflowComponent::fail(){
    state->reject(this);
}

void WorkflowComponent::cancel(){
    state->cancel(this);
}

WorkflowIterator* WorkflowComponent::createIterator(){
    return new depthFirstIterator(this);
}






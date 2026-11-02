#include "WorkflowComponent.h"
#include "DepthFirstIterator.h"
#include "ConcreteStages.h"


WorkflowComponent::WorkflowComponent() :
   mediator(nullptr), state(NotStarted::instance()) {}

//states are shared and never deleted, so there is nothing to clean up
WorkflowComponent::~WorkflowComponent() {}

void WorkflowComponent::setState(Stage* s){
    state = s; 
}

Stage* WorkflowComponent::getState(){
    return state;
}

void WorkflowComponent::setMediator(WorkflowOrchestrator* m){
    mediator = m;
}

//start now asks the state to start (before, it asked the state to execute)
void WorkflowComponent::start(){
   state->start(this);
}

void WorkflowComponent::complete(){
    state->complete(this);
}

//was called fail(), renamed to match Stage::reject
void WorkflowComponent::reject(){
    state->reject(this);
}

void WorkflowComponent::cancel(){
    state->cancel(this);
}

WorkflowIterator* WorkflowComponent::createIterator(){
    return new depthFirstIterator(this);
}

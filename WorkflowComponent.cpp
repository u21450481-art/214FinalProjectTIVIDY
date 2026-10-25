#include "WorkflowComponent.h"

void WorkflowComponent::setState(Stage* s){
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


WorkflowComponent* WorkflowComponent::clone(){
    
}




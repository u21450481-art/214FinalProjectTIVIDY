#include "StateTransitionCommand.h"
#include "Stage.h"
#include "WorkflowComponent.h"

StateTransitionCommand::StateTransitionCommand(WorkflowComponent* target): target(target), previous(nullptr), executed(false){}

StateTransitionCommand::~StateTransitionCommand(){}

//try the transition and report whether the state really changed.
bool StateTransitionCommand::execute(){
    if(target == nullptr){
        return false;
    }

    previous = target->getState(); //remember where we were
    applyTransition(); //2 the subclass tries start/complete/...

    if(target->getState() == previous){ //3 nothing moved
        executed = false;
        return false;
    }

    executed = true;
    return true;
}

//restore the remembered state directly.
bool StateTransitionCommand::undo(){
    if(!executed || target == nullptr){
        return false;
    }

    target->setState(previous);
    executed = false;
    return true;
}

WorkflowComponent* StateTransitionCommand::getTarget() const{
    return target;
}

std::string StateTransitionCommand::describeTransition(const std::string& verb) const{
    if(target == nullptr){
        return verb + " (no target)";
    }

    std::string text = verb + " '" + target->getName() + "'";
    if(executed && previous != nullptr){
        text += " (" + previous->getName() + " -> " + target->getState()->getName() + ")";
    }
    return text;
}

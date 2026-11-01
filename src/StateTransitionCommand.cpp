#include "StateTransitionCommand.h"

#include "Stage.h"
#include "WorkflowComponent.h"

//sets up the command with the component whose state will change
StateTransitionCommand::StateTransitionCommand(WorkflowComponent* target): target(target), previous(nullptr), executed(false){}

//cleans up the command (it does not own the component or the states)
StateTransitionCommand::~StateTransitionCommand(){}

//tries the transition and returns true only if the state really changed. Returns false if the current state does not allow the move, and then nothing is changed
bool StateTransitionCommand::execute(){
    if(target == nullptr){
        return false;
    }

    previous = target->getState();
    applyTransition();

    if(target->getState() == previous){
        executed = false;
        return false;
    }

    executed = true;
    return true;
}

//puts the previous state back, for example when a task was started by mistake. Returns false if the command was never executed
bool StateTransitionCommand::undo(){
    if(!executed || target == nullptr){
        return false;
    }

    target->setState(previous);
    executed = false;
    return true;
}

//returns the component this command works on
WorkflowComponent* StateTransitionCommand::getTarget() const{
    return target;
}

//builds the history text, for example Start 'Review' (NotStarted -> InProgress)
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

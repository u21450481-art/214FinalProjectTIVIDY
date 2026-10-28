#include "StartCommand.h"

#include "WorkflowComponent.h"

//sets up the command for one component
StartCommand::StartCommand(WorkflowComponent* target): StateTransitionCommand(target){}

//returns the history text for this command
std::string StartCommand::describe() const{
    return describeTransition("Start");
}

//asks the component to start (the current state decides if that is allowed)
void StartCommand::applyTransition(){
    getTarget()->start();
}

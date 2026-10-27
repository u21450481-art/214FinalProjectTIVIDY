#include "StartCommand.h"
#include "WorkflowComponent.h"

StartCommand::StartCommand(WorkflowComponent* target): StateTransitionCommand(target){}

std::string StartCommand::describe() const{
    return describeTransition("Start");
}

//the ONLY line that differs between the four transition commands
void StartCommand::applyTransition(){
    getTarget()->start();
}

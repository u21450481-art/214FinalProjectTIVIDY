#include "CompleteCommand.h"

#include "WorkflowComponent.h"

CompleteCommand::CompleteCommand(WorkflowComponent* target): StateTransitionCommand(target){}

std::string CompleteCommand::describe() const{
    return describeTransition("Complete");
}

//the ONLY line that differs between the four transition commands
void CompleteCommand::applyTransition(){
    getTarget()->complete();
}

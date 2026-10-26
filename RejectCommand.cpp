#include "RejectCommand.h"
#include "WorkflowComponent.h"

RejectCommand::RejectCommand(WorkflowComponent* target): StateTransitionCommand(target){}

//reuses the shared helper from the base class
std::string RejectCommand::describe() const{
    return describeTransition("Reject");
}

//the ONLY line that differs between the four transition commands
void RejectCommand::applyTransition(){
    getTarget()->reject();
}

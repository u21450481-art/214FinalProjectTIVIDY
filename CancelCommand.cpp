#include "CancelCommand.h"
#include "WorkflowComponent.h"

CancelCommand::CancelCommand(WorkflowComponent* target): StateTransitionCommand(target) {}

std::string CancelCommand::describe() const{
    return describeTransition("Cancel");
}

//the ONLY line that differs between the four transition commands.
void CancelCommand::applyTransition(){
    getTarget()->cancel();
}

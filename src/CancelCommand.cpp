#include "CancelCommand.h"

#include "WorkflowComponent.h"

//sets up the command for one component
CancelCommand::CancelCommand(WorkflowComponent* target): StateTransitionCommand(target){}

//returns the history text for this command
std::string CancelCommand::describe() const{
    return describeTransition("Cancel");
}

//asks the component to cancel (the current state decides if that is allowed)
void CancelCommand::applyTransition(){
    getTarget()->cancel();
}

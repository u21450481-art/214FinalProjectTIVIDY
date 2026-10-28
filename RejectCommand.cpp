#include "RejectCommand.h"

#include "WorkflowComponent.h"

//sets up the command for one component
RejectCommand::RejectCommand(WorkflowComponent* target): StateTransitionCommand(target){}

//returns the history text for this command
std::string RejectCommand::describe() const{
    return describeTransition("Reject");
}

//asks the component to reject (the current state decides if that is allowed)
void RejectCommand::applyTransition(){
    getTarget()->reject();
}

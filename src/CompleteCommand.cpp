#include "CompleteCommand.h"

#include "WorkflowComponent.h"

//sets up the command for one component
CompleteCommand::CompleteCommand(WorkflowComponent* target): StateTransitionCommand(target){}

//returns the history text for this command
std::string CompleteCommand::describe() const{
    return describeTransition("Complete");
}

//asks the component to complete (the current state decides if that is allowed)
void CompleteCommand::applyTransition(){
    getTarget()->complete();
}

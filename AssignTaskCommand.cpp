#include "AssignTaskCommand.h"

#include "Participant.h"
#include "Stage.h"
#include "Task.h"

AssignTaskCommand::AssignTaskCommand(Task* task, Participant* assignee): task(task), assignee(assignee), previous(nullptr), executed(false) {}

//change the assignee, remembering the old one.
bool AssignTaskCommand::execute(){
    if(task == nullptr || assignee == nullptr){
        return false; //nothing valid to assign
    }
    if(task->getState()->isFinished()){
        return false; //cannot hand out finished work
    }
    if(task->getAssignee() == assignee){
        return false; //no change
    }

    previous = task->getAssignee();
    task->setAssignee(assignee);
    executed = true;
    return true;
}

//put the old assignee back (null means the task goes back to unassigned).
bool AssignTaskCommand::undo(){
    if(!executed){
        return false;
    }
    task->setAssignee(previous);
    executed = false;
    return true;
}

std::string AssignTaskCommand::describe() const{
    if(task == nullptr || assignee == nullptr){
        return "Assign (invalid)";
    }
    return "Assign '" + task->getName() + "' to " + assignee->getName();
}

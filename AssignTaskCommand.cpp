#include "AssignTaskCommand.h"

#include "Participant.h"
#include "Stage.h"
#include "Task.h"

//sets up the command with the task and the person to give it to
AssignTaskCommand::AssignTaskCommand(Task* task, Participant* assignee): task(task), assignee(assignee), previous(nullptr), executed(false) {}

//gives the task to the participant and remembers who had it before. Returns false if the task or participant is missing, the task is finished, or it already belongs to that participant
bool AssignTaskCommand::execute(){
    if(task == nullptr || assignee == nullptr){
        return false;
    }
    if(task->getState()->isFinished()){
        return false;
    }
    if(task->getAssignee() == assignee){
        return false;
    }

    previous = task->getAssignee();
    if (previous != nullptr)
        previous->unassignTask(task);
    task->setAssignee(assignee);
    assignee->assignTask(task); // so the person's workload counts this task
    executed = true;
    return true;
}

//gives the task back to the previous person (or to nobody) in case the wrong person was picked
bool AssignTaskCommand::undo(){
    if(!executed){
        return false;
    }
    assignee->unassignTask(task);
    task->setAssignee(previous);
    if (previous != nullptr)
        previous->assignTask(task);
    executed = false;
    return true;
}

//returns the history text, for example Assign 'Review' to Amy
std::string AssignTaskCommand::describe() const{
    if(task == nullptr || assignee == nullptr){
        return "Assign (invalid)";
    }
    return "Assign '" + task->getName() + "' to " + assignee->getName();
}

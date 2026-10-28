#include "RecordDecisionCommand.h"

#include "Stage.h"
#include "Task.h"

//sets up the command with the task and the decision to store
RecordDecisionCommand::RecordDecisionCommand(Task* task, const std::string& outcome): task(task), outcome(outcome), previous(""), executed(false){}

//stores the decision on the task and remembers the old one. Returns false if the task is missing, the decision is empty or the task is finished
bool RecordDecisionCommand::execute(){
    if(task == nullptr || outcome.empty()){
        return false;
    }
    if(task->getState()->isFinished()){
        return false;
    }

    previous = task->getOutcome();
    task->setOutcome(outcome);
    executed = true;
    return true;
}

//puts the old decision back in case the wrong one was recorded
bool RecordDecisionCommand::undo(){
    if(!executed){
        return false;
    }
    task->setOutcome(previous);
    executed = false;
    return true;
}

//returns the history text, for example Record decision 'approved' on 'Review'
std::string RecordDecisionCommand::describe() const{
    if(task == nullptr){
        return "Record decision (invalid)";
    }
    return "Record decision '" + outcome + "' on '" + task->getName() + "'";
}

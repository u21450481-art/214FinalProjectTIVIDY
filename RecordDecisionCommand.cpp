#include "RecordDecisionCommand.h"

#include "Stage.h"
#include "Task.h"

RecordDecisionCommand::RecordDecisionCommand(Task* task, const std::string& outcome): task(task), outcome(outcome), previous(""), executed(false){}

//store the decision, remembering the previous one.
bool RecordDecisionCommand::execute(){
    if(task == nullptr || outcome.empty()){
        return false;
    }
    if(task->getState()->isFinished()){
        return false; //a finished task can no longer take a decision
    }

    previous = task->getOutcome();
    task->setOutcome(outcome);
    executed = true;
    return true;
}

bool RecordDecisionCommand::undo(){
    if(!executed){
        return false;
    }
    task->setOutcome(previous);
    executed = false;
    return true;
}

std::string RecordDecisionCommand::describe() const{
    if(task == nullptr){
        return "Record decision (invalid)";
    }
    return "Record decision '" + outcome + "' on '" + task->getName() + "'";
}

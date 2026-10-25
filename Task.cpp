#include "Task.h"
#include "WorkflowComponent.h"
#include "ConcreteStages.h"

#include <string>

Task::Task(int priority, std::string deadline):
 priority(priority), deadline(deadline) {
    NotStarted* s = new NotStarted();
    this->setState(s);
}


void Task::execute(){
    this->start();
}

void Task::setAssignee(Participant* p){
    assignee = p;
}

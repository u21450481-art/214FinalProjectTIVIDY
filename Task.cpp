#include "Task.h"
#include "WorkflowComponent.h"
#include "ConcreteStages.h"

#include <string>

Task::Task(int priority, std::string deadline, std::string name):
 priority(priority), deadline(deadline), taskName(name), assignee(nullptr) {
    NotStarted* s = new NotStarted();
    this->setState(s);
}
Task::~Task(){
}

std::string Task::getName(){
    return taskName;
}

void Task::execute(){
    this->start();
}

void Task::setAssignee(Participant* p){
    assignee = p;
}
#include "Task.h"
#include "WorkflowComponent.h"
#include "ConcreteStages.h"

#include <string>

Task::Task(int priority, std::string deadline, std::string name):
 priority(priority), deadline(deadline), taskName(name) {
    NotStarted* s = new NotStarted();
    this->setState(s);
}


void Task::execute(){
    this->start();
}

void Task::setAssignee(Participant* p){
    assignee = p;
}

vector<WorkflowComponent*> getChildren(){
  vector<WorkflowComponent*> v;
  return v;  
}

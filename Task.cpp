#include "Task.h"
#include "WorkflowComponent.h"
#include "ConcreteStages.h"

#include <string>

Task::Task(int priority, std::string deadline, std::string name) : assignee(nullptr), taskName(name), priority(priority), deadline(deadline) // reordered this bc of a reorder error
{
    NotStarted *s = new NotStarted();
    this->setState(s);
}
Task::~Task()
{
}

std::string Task::getName()
{
    return taskName;
}

void Task::execute()
{
    this->start();
}

void Task::setAssignee(Participant *p)
{
    assignee = p;
}

WorkflowComponent *Task::clone() const
{
    // makes fresh Task with the same name/details, starting in a fresh NotStarted() state
    return new Task(this->priority, this->deadline, this->taskName);
}
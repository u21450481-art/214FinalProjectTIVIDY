#include "Task.h"
#include "WorkflowComponent.h"
#include "ConcreteStages.h"

#include <string>

Task::Task(int priority, std::string deadline, std::string name, std::string role) 
:assignee(nullptr), taskName(name), priority(priority), deadline(deadline), outcome(""), requiredRole(role) {}

Task::~Task()
{
}

std::string Task::getName()
{
    return taskName;
}

// does the work of the task (the current state decides what that means)
void Task::execute()
{
    getState()->handleExecute(this);
}

void Task::setAssignee(Participant *p)
{
    assignee = p;
}

Participant *Task::getAssignee()
{
    return assignee;
}

void Task::setOutcome(const std::string &o)
{
    outcome = o;
}

std::string Task::getOutcome()
{
    return outcome;
}

const std::string & Task::getRequiredRole() const { return requiredRole; }

void Task::setBranchCondition(const std::string &c) { branchCondition = c; }
std::string Task::getBranchCondition() const { return branchCondition; }

WorkflowComponent *Task::clone() const
{
    // ew Task starts NotStarted with no assignee or outcome (fresh run).
    Task *copy = new Task(priority, deadline, taskName,requiredRole);
    copy->branchCondition = branchCondition;
    return copy;
}
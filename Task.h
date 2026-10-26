#ifndef TASK_H
#define TASK_H

#include "WorkflowComponent.h"
#include "Participant.h"
#include <string>


class Task: public WorkflowComponent{

    Participant* assignee;
    std::string taskName;
    int priority;
    std::string deadline;

    public:
    Task(int priority, std::string deadline, std::string name);
    ~Task();
    std::string getName();
    void execute();
    void setAssignee(Participant* p);
};

#endif
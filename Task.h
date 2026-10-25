#ifndef TASK_H
#define TASK_H

#include "WorkflowComponent.h"
#include <string>

class Participant{};

class Task: public WorkflowComponent{

    Participant* assigne;
    std::string priority;
    std::string deadline;

    public:
    void execute();
    void setAssignee();
};

#endif
#ifndef TASK_H
#define TASK_H

#include "WorkflowComponent.h"
#include "Participant.h"
#include <string>

class Task : public WorkflowComponent
{

    Participant *assignee;
    std::string taskName;
    int priority;
    std::string deadline;
    std::string outcome; // the decision stored on the task, for example "approved"

    std::string branchCondition;

public:
    Task(int priority, std::string deadline, std::string name);
    ~Task();
    std::string getName();
    void execute();
    void setAssignee(Participant *p);
    Participant *getAssignee();
    void setOutcome(const std::string &o);
    std::string getOutcome();

    // added for builder
    void setBranchCondition(const std::string &c);
    std::string getBranchCondition() const;
    WorkflowComponent *clone() const override;

    const std::string &getRequiredRole() const { return "requiredRole"; }
};

#endif

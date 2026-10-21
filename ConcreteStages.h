#ifndef CONCRETE_STAGES_H
#define CONCRETE_STAGES_H

#include "Stage.h"

//Not Stated
class NotStarted : public Stage{
public:
    void handleExecute(WorkflowComponent* c) override;
    void start(WorkflowComponent* c) override;
    void cancel(WorkflowComponent* c) override;
    std::string getName() const override {return "NotStarted";}
};

//InProgress
class InProgress : public Stage{
public:
    void handleExecute(WorkflowComponent* c) override;
    void complete(WorkflowComponent* c) override;
    void reject(WorkflowComponent* c) override;
    void cancel(WorkflowComponent* c) override;
    std::string getName() const override {return "InProgress";}
};

//Complete (Terminal)
class Complete : public Stage{
public:
    void handleExecute(WorkflowComponent* c) override;
    std::string getName() const override {return "Complete";}
};

//Rejected (Terminal)
class Rejected : public Stage{
public:
    void handleExecute(WorkflowComponent* c) override;
    std::string getName() const override {return "Rejected";}
};

class Cancelled : public Stage{
public:
    void handleExecute(WorkflowComponent* c) override;
    std::string getName() const override {return "Cancelled";}
};

#endif


#ifndef CONCRETE_STAGES_H
#define CONCRETE_STAGES_H

#include "Stage.h"

//The states hold no data, so ONE shared instance of each is enough. instance() returns it.
//Nobody news or deletes a state, so a command can safely remember the previous state.

//Not Stated
class NotStarted : public Stage{
public:
    static Stage* instance();
    void handleExecute(WorkflowComponent* c) override;
    void start(WorkflowComponent* c) override;
    void cancel(WorkflowComponent* c) override;
    std::string getName() const override {return "NotStarted";}
};

//InProgress
class InProgress : public Stage{
public:
    static Stage* instance();
    void handleExecute(WorkflowComponent* c) override;
    void complete(WorkflowComponent* c) override;
    void reject(WorkflowComponent* c) override;
    void cancel(WorkflowComponent* c) override;
    std::string getName() const override {return "InProgress";}
};

//Complete (Terminal)
class Complete : public Stage{
public:
    static Stage* instance();
    void handleExecute(WorkflowComponent* c) override;
    std::string getName() const override {return "Complete";}
    bool isFinished() const override {return true;}
};

//Rejected (Terminal)
class Rejected : public Stage{
public:
    static Stage* instance();
    void handleExecute(WorkflowComponent* c) override;
    std::string getName() const override {return "Rejected";}
    bool isFinished() const override {return true;}
};

//Cancelled (Terminal)
class Cancelled : public Stage{
public:
    static Stage* instance();
    void handleExecute(WorkflowComponent* c) override;
    std::string getName() const override {return "Cancelled";}
    bool isFinished() const override {return true;}
};

#endif

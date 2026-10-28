#ifndef REJECTCOMMAND_H
#define REJECTCOMMAND_H

#include "StateTransitionCommand.h"

//moves a component from InProgress to Rejected (the exception path)
class RejectCommand: public StateTransitionCommand{
    protected:
        //calls target->reject(), the current state decides whether that is allowed
        void applyTransition();

    public:
        RejectCommand(WorkflowComponent* target); //target: the component to Reject

        std::string describe() const;
};

#endif

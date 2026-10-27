#ifndef REJECTCOMMAND_H
#define REJECTCOMMAND_H

#include "StateTransitionCommand.h"

/** InProgress -> Rejected. The exception path: the work was refused (failed approval, denied claim). */
class RejectCommand: public StateTransitionCommand{
    protected:
        //calls target->reject()
        void applyTransition();

    public:
        RejectCommand(WorkflowComponent* target); //target: the component to Reject
        std::string describe() const;
};

#endif

#ifndef CANCELCOMMAND_H
#define CANCELCOMMAND_H

#include "StateTransitionCommand.h"

//NotStarted or InProgress: Cancelled. Records that the work was abandoned.
class CancelCommand: public StateTransitionCommand{
    protected:
        void applyTransition();
        
    public:
        CancelCommand(WorkflowComponent* target); //target :the component to Cancel.
        std::string describe() const;
};

#endif

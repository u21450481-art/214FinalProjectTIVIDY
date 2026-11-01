#ifndef CANCELCOMMAND_H
#define CANCELCOMMAND_H

#include "StateTransitionCommand.h"

//moves a component from NotStarted or InProgress to Cancelled
class CancelCommand: public StateTransitionCommand{
    protected:
        //calls target->cancel(), the current state decides whether that is allowed
        void applyTransition();

    public:
        CancelCommand(WorkflowComponent* target); //target: the component to Cancel

        std::string describe() const;
};

#endif

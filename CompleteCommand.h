#ifndef COMPLETECOMMAND_H
#define COMPLETECOMMAND_H

#include "StateTransitionCommand.h"

//moves a component from InProgress to Complete
class CompleteCommand: public StateTransitionCommand{
    protected:
        //calls target->complete(), the current state decides whether that is allowed
        void applyTransition();

    public:
        CompleteCommand(WorkflowComponent* target); //target: the component to Complete

        std::string describe() const;
};

#endif

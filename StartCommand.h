#ifndef STARTCOMMAND_H
#define STARTCOMMAND_H

#include "StateTransitionCommand.h"

//moves a component from NotStarted to InProgress
class StartCommand: public StateTransitionCommand{
    protected:
        //calls target->start(), the current state decides whether that is allowed
        void applyTransition();

    public:
        StartCommand(WorkflowComponent* target); //target: the component to Start

        std::string describe() const;
};

#endif

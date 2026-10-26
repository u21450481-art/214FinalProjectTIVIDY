#ifndef STARTCOMMAND_H
#define STARTCOMMAND_H

#include "StateTransitionCommand.h"

/** NotStarted -> InProgress. Records that work has begun, refused in any other state. */
class StartCommand: public StateTransitionCommand{
    protected:
        //calls target->start()
        void applyTransition();

    public:
        StartCommand(WorkflowComponent* target); //target: the component to Start
        std::string describe() const;
};

#endif

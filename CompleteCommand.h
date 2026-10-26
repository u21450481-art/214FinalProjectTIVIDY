#ifndef COMPLETECOMMAND_H
#define COMPLETECOMMAND_H

#include "StateTransitionCommand.h"

/** InProgress -> Complete. Records that work finished, usually what releases dependent tasks. */
class CompleteCommand: public StateTransitionCommand{
    protected:
        //calls target->complete()
        void applyTransition();

    public:
        CompleteCommand(WorkflowComponent* target); //target: the component to Complete
        std::string describe() const;
};

#endif

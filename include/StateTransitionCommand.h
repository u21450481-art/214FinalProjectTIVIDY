#ifndef STATETRANSITIONCOMMAND_H
#define STATETRANSITIONCOMMAND_H

#include <string>
#include "Command.h"

class WorkflowComponent;
class Stage;

//base class for the commands that move a component to another state (start, complete, reject, cancel)
class StateTransitionCommand: public Command{
    private:
        WorkflowComponent* target; //component whose state we change
        Stage* previous; //state before execute() (shared instance)
        bool executed; //true while the change is applied

    protected:
        //subclasses call the one transition they stand for (start, complete...)
        virtual void applyTransition() = 0;

        //builds text like "Start 'Review' (NotStarted -> InProgress)"
        std::string describeTransition(const std::string& verb) const;

        WorkflowComponent* getTarget() const;

    public:
        StateTransitionCommand(WorkflowComponent* target);
        virtual ~StateTransitionCommand();

        bool execute();

        bool undo();
};

#endif

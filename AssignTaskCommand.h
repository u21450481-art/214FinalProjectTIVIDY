#ifndef ASSIGNTASKCOMMAND_H
#define ASSIGNTASKCOMMAND_H

#include <string>
#include "Command.h"

class Task;
class Participant;

//Gives a task to a participant, and can give it back.
class AssignTaskCommand: public Command{
    private:
        Task* task;
        Participant* assignee;
        Participant* previous;
        bool executed;
    
    public:
        AssignTaskCommand(Task* task, Participant* assignee); //tasK:task to assign , assignee:who were asiigning the task to.

        /**
        * Sets the assignee. Returns false when: task or assignee is null, the task
        * is already finished, or it is already assigned to this participant.
        */
        bool execute();

        /** Restores the previous assignee (which may be "nobody"). */
        bool undo();
        std::string describe() const;
};

#endif

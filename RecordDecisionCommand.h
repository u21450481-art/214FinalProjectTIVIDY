#ifndef RECORDDECISIONCOMMAND_H
#define RECORDDECISIONCOMMAND_H

#include <string>
#include "Command.h"

class Task;

//stores a decision such as approved or rejected on a task and can restore the old one
class RecordDecisionCommand: public Command{
    private:
        Task* task;
        std::string outcome;
        std::string previous; //outcome before execute()
        bool executed;

    public:
        RecordDecisionCommand(Task* task, const std::string& outcome);

        bool execute();

        bool undo();
        std::string describe() const;
};

#endif

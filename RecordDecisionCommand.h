#ifndef RECORDDECISIONCOMMAND_H
#define RECORDDECISIONCOMMAND_H

#include <string>
#include "Command.h"

class Task;

//stores a decision (for example "approved" or "rejected") on a task.
class RecordDecisionCommand: public Command{
    private:
        Task* task;
        std::string outcome;
        std::string previous; //outcome before execute()
        bool executed;

    public:
        RecordDecisionCommand(Task* task, const std::string& outcome);
        bool execute();  //saves the old outcome and stores the new one.
        bool undo(); //restores the old outcome, or clears it if there was none.
        std::string describe() const;
};

#endif

#ifndef COMMAND_H
#define COMMAND_H

#include <string>

//one action the engine can run, undo and describe. Every action (change a state, assign a task, set data) is a Command
class Command{
    private:
        //copying is switched off: some commands own other commands and history deletes them once
        Command(const Command&) = delete;
        Command& operator=(const Command&) = delete;

    protected:
        Command();

    public:
        virtual ~Command();

        virtual bool execute() = 0;

        virtual bool undo() = 0;

        virtual std::string describe() const = 0;
};

#endif

#ifndef COMMAND_H
#define COMMAND_H

#include <string>

//Abstract Command: one action the workflow engine can perform, reverse and describe.
class Command{
    protected:
        Command();

    private:
        Command(const Command&) = delete;
        Command& operator=(const Command&) = delete;
        
    public:
        virtual ~Command();
        virtual bool execute() = 0;
        virtual bool undo() = 0;
        //what this command does
        virtual std::string describe() const = 0;
};

#endif

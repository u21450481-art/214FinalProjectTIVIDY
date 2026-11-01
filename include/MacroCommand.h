#ifndef MACROCOMMAND_H
#define MACROCOMMAND_H

#include <string>
#include <vector>
#include "Command.h"


class MacroCommand: public Command{
    private:
        std::string label; //short name shown in the history
        std::vector<Command*> steps;
        int doneCount; //how many leading steps are currently applied

    public:
        MacroCommand(const std::string& label);
        ~MacroCommand();
        void add(Command* step);//adds a step at the end.
        bool execute(); //runs the steps in order 
        bool undo();
        std::string describe() const;
};

#endif

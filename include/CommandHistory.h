#ifndef COMMANDHISTORY_H
#define COMMANDHISTORY_H

#include <string>
#include <vector>
#include "Command.h"

//runs commands and remembers them so they can be undone, redone and listed. It owns the commands it is given and deletes them
class CommandHistory{
    private:
        std::vector<Command*> commands; //owned, oldest first
        int cursor; //how many of the commands are currently done

        //deletes every command after the cursor (the ones that could still be redone)
        void discardRedoTail();

        //history owns raw pointers, so copying it would delete things twice
        CommandHistory(const CommandHistory&) = delete;
        CommandHistory& operator=(const CommandHistory&) = delete;

    public:
        CommandHistory();
        ~CommandHistory();

        bool run(Command* c);

        bool undo();

        bool redo();

        bool canUndo() const;
        bool canRedo() const;

        int size() const;

        std::vector<std::string> getHistory() const;
};

#endif

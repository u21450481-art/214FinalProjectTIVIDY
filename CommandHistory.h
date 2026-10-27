#ifndef COMMANDHISTORY_H
#define COMMANDHISTORY_H

#include <string>
#include <vector>
#include "Command.h"

/**
 * Runs commands and remembers them so they can be undone, redone and listed (the Invoker).
 * Everything is kept in ONE list plus a cursor:
 *   commands = [A, B, C, D]   cursor = 3   ->  A, B, C are done and D is undone
 */

class CommandHistory{
    private:
        std::vector<Command*> commands;
        int cursor; //how many of the commands are currently done
        void discardRedoTail(); //deletes every command after the cursor

        //history owns raw pointers, so copying it would delete things twice
        CommandHistory(const CommandHistory&) = delete;
        CommandHistory& operator=(const CommandHistory&) = delete;

    public:
        CommandHistory();
        ~CommandHistory();
        bool run(Command* c); 
        bool undo(); //undoes the newest done command. Returns false if there is nothing to undo.
        bool redo(); //executes the newest undone command again
        bool canUndo() const;
        bool canRedo() const;
        int size() const;  //How many commands are stored (done and undone)
        std::vector<std::string> getHistory() const;
};

#endif

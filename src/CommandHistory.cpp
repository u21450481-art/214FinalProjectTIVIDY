#include "CommandHistory.h"

//starts an empty history with nothing stored and nothing done
CommandHistory::CommandHistory(): cursor(0){}

//deletes every command still stored, because the history owns them
CommandHistory::~CommandHistory(){
    for(int i = 0; i < (int)commands.size(); i++){
        delete commands[i];
    }
}

//runs a command and keeps it only if it worked. The history takes ownership of the command, so it deletes it if it fails. A new command also deletes any undone commands that could have been redone
bool CommandHistory::run(Command* c){
    if(c == nullptr){
        return false;
    }

    if(!c->execute()){
        delete c;
        return false;
    }

    discardRedoTail();
    commands.push_back(c);
    cursor++;
    return true;
}

//undoes the most recent command, for example when a wrong value was entered. Returns false if there is nothing to undo
bool CommandHistory::undo(){
    if(cursor == 0){
        return false;
    }

    Command* latest = commands[cursor - 1];
    if(!latest->undo()){
        return false;
    }

    cursor--;
    return true;
}

//does the most recently undone command again. Returns false if there is nothing to redo
bool CommandHistory::redo(){
    if(cursor >= (int)commands.size()){
        return false;
    }

    Command* next = commands[cursor];
    if(!next->execute()){
        return false;
    }

    cursor++;
    return true;
}

//tells whether there is something to undo
bool CommandHistory::canUndo() const{
    return cursor > 0;
}

//tells whether there is something to redo
bool CommandHistory::canRedo() const{
    return cursor < (int)commands.size();
}

//returns how many commands are stored (done and undone)
int CommandHistory::size() const{
    return (int)commands.size();
}

//returns one line per command, oldest first, and marks the undone ones with (undone)
std::vector<std::string> CommandHistory::getHistory() const{
    std::vector<std::string> lines;
    for(int i = 0; i < (int)commands.size(); i++){
        std::string line = commands[i]->describe();
        if(i >= cursor){
            line = "(undone) " + line;
        }
        lines.push_back(line);
    }
    return lines;
}

//deletes the commands that were undone, because a new command replaces them
void CommandHistory::discardRedoTail(){
    for(int i = cursor; i < (int)commands.size(); i++){
        delete commands[i];
    }
    commands.resize(cursor);
}

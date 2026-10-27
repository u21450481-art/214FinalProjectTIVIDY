#include "CommandHistory.h"

//start empty, nothing stored and nothing done
CommandHistory::CommandHistory(): cursor(0){}

//history owns every command in the list, so it deletes them all here
CommandHistory::~CommandHistory(){
    for(int i = 0; i < (int)commands.size(); i++){
        delete commands[i];
    }
}

//run executes the command and keeps it ONLY if it worked.
//null command: returns false.
//execute fails: history deletes the command, returns false, redo list is left alone.
//execute works: old redo commands are deleted (that future no longer exists),
//the command is stored and true is returned.
bool CommandHistory::run(Command* c){
    if(c == nullptr){
        return false;
    }

    if(!c->execute()){
        delete c; //we own it and it did nothing, so it is not worth keeping
        return false;
    }

    discardRedoTail(); //a brand new action cancels anything that was undone before
    commands.push_back(c);
    cursor++;
    return true;
}

//undo reverses the newest done command and moves the cursor back by one.
//nothing done: returns false. The command's own undo fails: false and the cursor stays.
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

//redo executes the newest undone command again and moves the cursor forward by one.
//nothing undone: returns false. Execute fails now (the world changed): false and the cursor stays.
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

bool CommandHistory::canUndo() const{
    return cursor > 0;
}

bool CommandHistory::canRedo() const{
    return cursor < (int)commands.size();
}

int CommandHistory::size() const{
    return (int)commands.size();
}

//getHistory asks every command to describe itself.
//commands after the cursor were undone, so they get a marker in front.
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

//deletes the commands after the cursor and shrinks the list to match
void CommandHistory::discardRedoTail(){
    for(int i = cursor; i < (int)commands.size(); i++){
        delete commands[i];
    }
    commands.resize(cursor);
}
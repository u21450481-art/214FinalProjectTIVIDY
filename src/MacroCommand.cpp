#include "MacroCommand.h"

MacroCommand::MacroCommand(const std::string& label): label(label), doneCount(0){}

MacroCommand::~MacroCommand(){
    for(int i = 0; i < (int)steps.size(); i++){
        delete steps[i];
    }
}

void MacroCommand::add(Command* step){
    if(step != nullptr){
        steps.push_back(step);
    }
}

//empty macro: false. Step i fails: undo steps i-1 down
bool MacroCommand::execute(){
    if(steps.empty()){
        return false;
    }

    doneCount = 0;
    for(int i = 0; i < (int)steps.size(); i++){
        if(!steps[i]->execute()){
            //roll back what already happened, newest first
            while(doneCount > 0){
                doneCount--;
                steps[doneCount]->undo();
            }
            return false;
        }
        doneCount++;
    }
    return true;
}

//undo in reverse order so each step undoes the next step's changes
bool MacroCommand::undo(){
    if(doneCount == 0){
        return false;
    }

    bool allUndone = true;
    while(doneCount > 0){
        doneCount--;
        if(!steps[doneCount]->undo()){
            allUndone = false;
        }
    }
    return allUndone;
}

std::string MacroCommand::describe() const{
    return label + " [" + std::to_string((int)steps.size()) + " steps]";
}

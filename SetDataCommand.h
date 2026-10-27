#ifndef SETDATACOMMAND_H
#define SETDATACOMMAND_H

#include <string>
#include "Command.h"

class DataPayload;

// writes one value into a workflow instance's data, and can reverse it.
class SetDataCommand: public Command{
    private:
        DataPayload* payload;
        std::string key;
        std::string newValue;
        std::string oldValue; //value before execute(), only meaningful if hadOld
        bool hadOld; //did the key exist before execute()?
        bool executed;

    public:
        SetDataCommand(DataPayload* payload, const std::string& key, const std::string& newValue);
        bool execute(); //saves the old value (if any) and writes the new one
        bool undo(); //writes the old value back, or removes the key if it did not exist before
        std::string describe() const;
};

#endif

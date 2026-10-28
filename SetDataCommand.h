#ifndef SETDATACOMMAND_H
#define SETDATACOMMAND_H

#include <string>
#include "Command.h"

class DataPayload;

//writes one value into the instance data and can put the old value back
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

        bool execute();

        bool undo();
        std::string describe() const;
};

#endif

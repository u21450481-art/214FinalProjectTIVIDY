#include "SetDataCommand.h"

#include "DataPayload.h"

//sets up the command with the data, the key and the new value
SetDataCommand::SetDataCommand(DataPayload* payload, const std::string& key, const std::string& newValue): payload(payload), key(key), newValue(newValue), oldValue(""), hadOld(false), executed(false){}

//saves the old value, then writes the new value into the data. Returns false if there is no data or the key is empty
bool SetDataCommand::execute(){
    if(payload == nullptr || key.empty()){
        return false;
    }

    hadOld = payload->has(key);
    if(hadOld){
        oldValue = payload->get(key);
    }

    payload->set(key, newValue);
    executed = true;
    return true;
}

//puts the old value back, or removes the key if it did not exist before, in case a wrong value was entered
bool SetDataCommand::undo(){
    if(!executed){
        return false;
    }

    if(hadOld){
        payload->set(key, oldValue);
    }else{
        payload->remove(key);
    }
    executed = false;
    return true;
}

//returns the history text, for example Set data 'amount' = '1500'
std::string SetDataCommand::describe() const{
    return "Set data '" + key + "' = '" + newValue + "'";
}

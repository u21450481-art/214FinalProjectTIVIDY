#include "SetDataCommand.h"
#include "DataPayload.h"

SetDataCommand::SetDataCommand(DataPayload* payload, const std::string& key, const std::string& newValue): payload(payload), key(key), newValue(newValue), oldValue(""), hadOld(false), executed(false){}

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

//two cases: the key existed before (restore it) or it did not (remove it).
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

std::string SetDataCommand::describe() const{
    return "Set data '" + key + "' = '" + newValue + "'";
}

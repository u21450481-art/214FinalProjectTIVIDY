#include "Stage.h"
#include "Log.h"

void Stage::start(WorkflowComponent* c){
    TLOG("[State Warning] Cannot start from state: " << getName());
}

void Stage::complete(WorkflowComponent* c){
    TLOG("[State Warning] Cannot complete from state: " << getName());
}

void Stage::reject(WorkflowComponent* c){
    TLOG("[State Warning] Cannot reject from state: " << getName());
}

void Stage::cancel(WorkflowComponent* c){
    TLOG("[State Warning] Cannot cancel from state: " << getName());
}

bool Stage::isFinished() const{
    return false;
}

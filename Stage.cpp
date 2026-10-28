#include "Stage.h"
#include <iostream>

void Stage::start(WorkflowComponent* c){
    std::cout << "[State Warning] Cannot start from state: " << getName() << "\n";
}

void Stage::complete(WorkflowComponent* c){
    std::cout << "[State Warning] Cannot complete from state: " << getName() << "\n";
}

void Stage::reject(WorkflowComponent* c){
    std::cout << "[State Warning] Cannot reject from state: " << getName() << "\n";
}

void Stage::cancel(WorkflowComponent* c){
    std::cout << "[State Warning] Cannot cancel from state: " << getName() << "\n";
}

bool Stage::isFinished() const{
    return false;
}

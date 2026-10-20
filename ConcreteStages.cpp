#include "ConcreteStages.h"
#include <iostream>

//(WorkflowComponent provdes c->setState(Stage* s) thats why i commented  it out for now)

//================NotStarted=========================
void NotStarted::handleExecute(WorkflowComponent* c){
    std::cout << "[Stage: NotStarted] Initiating execution...\n";
    this->start(c);
}

void NotStarted::start(WorkflowComponent* c){
    std::cout << "[Stage Transition] NotStarted -> InProgress\n";
    //c->setState(new InProgress());
}

void NotStarted::cancel(WorkflowComponent* c){
    std::cout << "[Stage Transition] NotStarted -> Cancelled\n";
    //c->setState(new Cancelled());
}

//=================InProgress====================
void InProgress::handleExecute(WorkflowComponent* c){
    std::cout << "[Stage: InProgress] Performin component work...\n";
}

void InProgress::complete(WorkflowComponent* c){
    std::cout << "[Stage Transition] InProgress -> Complete\n";
    //c->setState(new Complete());
}

void InProgress::reject(WorkflowComponent* c){
    std::cout << "[Stage Transition] InProgress -> Rejected\n";
    //c->setState(new Rejected());
}

void InProgress::cancel(WorkflowComponent* c){
    std::cout << "[Stage Transition] InProgress -> Cancelled\n";
    //c->setState(new Cancelled());
}

//=================Complete====================
void Complete::handleExecute(WorkflowComponent* c){
    std::cout << "[Stage: Complete] Component is already Complete. Execution blocked\n";
}

//=================Rejected====================
void Rejected::handleExecute(WorkflowComponent* c){
    std::cout << "[Stage: Rejected] Component was Rejected. Execution blocked.\n";
}

//=================Cancelled====================
void Cancelled::handleExecute(WorkflowComponent* c){
    std::cout << "[Stage: Cancelled] Component was Cancelled. Execution blocked.\n";
}

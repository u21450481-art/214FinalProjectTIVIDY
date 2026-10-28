#include "ConcreteStages.h"
#include "WorkflowComponent.h"
#include <iostream>

//================NotStarted=========================
Stage* NotStarted::instance(){
    static NotStarted theOne;
    return &theOne;
}

//no longer starts itself: starting must go through StartCommand so it is recorded in the history
void NotStarted::handleExecute(WorkflowComponent* c){
    std::cout << "[Stage: NotStarted] Not started yet. Start it first.\n";
}

void NotStarted::start(WorkflowComponent* c){
    std::cout << "[Stage Transition] NotStarted -> InProgress\n";
    c->setState(InProgress::instance());
}

void NotStarted::cancel(WorkflowComponent* c){
    std::cout << "[Stage Transition] NotStarted -> Cancelled\n";
    c->setState(Cancelled::instance());
}

//=================InProgress====================
Stage* InProgress::instance(){
    static InProgress theOne;
    return &theOne;
}

void InProgress::handleExecute(WorkflowComponent* c){
    std::cout << "[Stage: InProgress] Performing component work...\n";
}

void InProgress::complete(WorkflowComponent* c){
    std::cout << "[Stage Transition] InProgress -> Complete\n";
    c->setState(Complete::instance());
}

void InProgress::reject(WorkflowComponent* c){
    std::cout << "[Stage Transition] InProgress -> Rejected\n";
    c->setState(Rejected::instance());
}

void InProgress::cancel(WorkflowComponent* c){
    std::cout << "[Stage Transition] InProgress -> Cancelled\n";
    c->setState(Cancelled::instance());
}

//=================Complete====================
Stage* Complete::instance(){
    static Complete theOne;
    return &theOne;
}

void Complete::handleExecute(WorkflowComponent* c){
    std::cout << "[Stage: Complete] Component is already Complete. Execution blocked\n";
}

//=================Rejected====================
Stage* Rejected::instance(){
    static Rejected theOne;
    return &theOne;
}

void Rejected::handleExecute(WorkflowComponent* c){
    std::cout << "[Stage: Rejected] Component was Rejected. Execution blocked.\n";
}

//=================Cancelled====================
Stage* Cancelled::instance(){
    static Cancelled theOne;
    return &theOne;
}

void Cancelled::handleExecute(WorkflowComponent* c){
    std::cout << "[Stage: Cancelled] Component was Cancelled. Execution blocked.\n";
}

#include "ConcreteStages.h"
#include "WorkflowComponent.h"
#include "Log.h"

//================NotStarted=========================
Stage* NotStarted::instance(){
    static NotStarted theOne;
    return &theOne;
}

//no longer starts itself: starting must go through StartCommand so it is recorded in the history
void NotStarted::handleExecute(WorkflowComponent* c){
    TLOG("[Stage: NotStarted] Not started yet. Start it first.");
}

void NotStarted::start(WorkflowComponent* c){
    TLOG("[Stage Transition] NotStarted -> InProgress");
    c->setState(InProgress::instance());
}

void NotStarted::cancel(WorkflowComponent* c){
    TLOG("[Stage Transition] NotStarted -> Cancelled");
    c->setState(Cancelled::instance());
}

//=================InProgress====================
Stage* InProgress::instance(){
    static InProgress theOne;
    return &theOne;
}

void InProgress::handleExecute(WorkflowComponent* c){
    TLOG("[Stage: InProgress] Performing component work...");
}

void InProgress::complete(WorkflowComponent* c){
    TLOG("[Stage Transition] InProgress -> Complete");
    c->setState(Complete::instance());
}

void InProgress::reject(WorkflowComponent* c){
    TLOG("[Stage Transition] InProgress -> Rejected");
    c->setState(Rejected::instance());
}

void InProgress::cancel(WorkflowComponent* c){
    TLOG("[Stage Transition] InProgress -> Cancelled");
    c->setState(Cancelled::instance());
}

//=================Complete====================
Stage* Complete::instance(){
    static Complete theOne;
    return &theOne;
}

void Complete::handleExecute(WorkflowComponent* c){
    TLOG("[Stage: Complete] Component is already Complete. Execution blocked");
}

//=================Rejected====================
Stage* Rejected::instance(){
    static Rejected theOne;
    return &theOne;
}

void Rejected::handleExecute(WorkflowComponent* c){
    TLOG("[Stage: Rejected] Component was Rejected. Execution blocked.");
}

//=================Cancelled====================
Stage* Cancelled::instance(){
    static Cancelled theOne;
    return &theOne;
}

void Cancelled::handleExecute(WorkflowComponent* c){
    TLOG("[Stage: Cancelled] Component was Cancelled. Execution blocked.");
}

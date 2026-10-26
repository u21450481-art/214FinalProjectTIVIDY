#include "Department.h"

void Department::add(Participant* p){
 participants.push_back(p);
}

void Department::remove(Participant* p){

}

vector<Participant*> Department::getChildren(){
    return participants;
}
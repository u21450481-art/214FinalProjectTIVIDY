#include "Department.h"

#include <algorithm>

Department::Department(std::string name): departmentName(name){};

Department::~Department(){
    for (auto* c : participants) delete c;
}

std::string Department::getName(){
    return departmentName;
}

void Department::Operation(){
    for (auto* c : participants) c->Operation();
}

void Department::add(Participant* p){
 participants.push_back(p);
}

void Department::remove(Participant* p){
    participants.erase(std::remove(participants.begin(), participants.end(), p), participants.end());
}

std::vector<Participant*> Department::getChildren(){
    return participants;
}
#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "Participant.h"

class Employee : public Participant{
    
public:
    void Operation();
    void add(Participant* p){};
    void remove(Participant* p){};
    vector<Participant*> getChildren();

};

#endif
#ifndef DEPARTMENT_H
#define DEPARTMENT_H


#include "Participant.h"

#include <vector>
using namespace std;

class Department : public Participant{
    vector<Participant*> participants;

public:
    void Operation();
    void add(Participant* p);
    void remove(Participant* p);
    vector<Participant*> getChildren();

};
#endif
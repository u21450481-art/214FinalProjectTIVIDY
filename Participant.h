#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <vector>
using namespace std;

class Participant{

public:
    virtual void Operation() = 0;
    virtual void add(Participant* p) = 0;
    virtual void remove(Participant* p) = 0;
    virtual vector<Participant*> getChildren() = 0;
};

#endif
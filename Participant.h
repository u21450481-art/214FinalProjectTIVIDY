#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <string>
#include <vector>
using namespace std;

class Participant{

public:
    virtual ~Participant() {}
    virtual void Operation() = 0;
    virtual std::string getName() = 0;
    virtual void add(Participant* p) {};
    virtual void remove(Participant* p) {};
    virtual vector<Participant*> getChildren() {return {};};
};

#endif
#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <string>
#include <vector>
using namespace std;

class Task;

class Participant
{

public:
    virtual ~Participant() {}
    virtual void Operation() = 0;
    virtual std::string getName() = 0;
    virtual void add(Participant *p) {};
    virtual void remove(Participant *p) {};
    virtual vector<Participant *> getChildren() { return {}; };

    virtual std::string getRole() const { return ""; }
    virtual int getWorkload() const { return 0; }
    virtual int getExperience() const { return 0; }

    // Hooks used when work is handed out. Only participants that can hold tasks override them.
    virtual void assignTask(Task *t) {}
    virtual void unassignTask(Task *t) {}
};

#endif
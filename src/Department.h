#ifndef DEPARTMENT_H
#define DEPARTMENT_H


#include "Participant.h"

#include <vector>
#include <string>

using namespace std;

class Department : public Participant{
    vector<Participant*> participants;
    string departmentName;

public:
    Department(string);
    ~Department();
    string getName();
    void Operation() override;
    void add(Participant* p) override;
    void remove(Participant* p) override;
    vector<Participant*> getChildren() override;

};
#endif
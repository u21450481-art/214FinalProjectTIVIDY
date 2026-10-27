#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "Participant.h"
#include "Task.h"

#include <vector>
#include <string>

class Employee : public Participant{
    std::string employeeName;
    std::vector<Task*> tasks;

public:
    Employee(std::string);
    ~Employee();
    void assignTask(Task*);
    std::string getName();
    void Operation();

};

#endif
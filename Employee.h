#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "Participant.h"
#include "Task.h"

#include <vector>
#include <string>

class Employee : public Participant{
    std::vector<Task*> tasks;
    std::string role;
    int experience;
    int workload;

public:
    Employee(std::string, int);
    ~Employee();
    void assignTask(Task*);
    std::string getRole() const override;
    int getExperience() const override;
    int getWorkload() const override;
    
    void Operation();

};

#endif
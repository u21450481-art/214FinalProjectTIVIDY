#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "Participant.h"
#include "Task.h"

#include <vector>
#include <string>

class Employee : public Participant{
    std::vector<Task*> tasks;
    std::string name;
    std::string role;
    int experience;

public:
    Employee(std::string name, std::string role, int experience);
    ~Employee();
    std::string getName() override;
    void assignTask(Task*) override;
    void unassignTask(Task*) override;
    std::string getRole() const override;
    int getExperience() const override;
    int getWorkload() const override;
    
    void Operation();

};

#endif
#include "Employee.h"

#include <algorithm>
#include <iostream>

Employee::Employee(std::string name, std::string role, int experience): name(name), role(role), experience(experience){}

Employee::~Employee(){}

std::string Employee::getRole() const{
    return role;
}

int Employee::getExperience() const{
    return experience;
}
std::string Employee::getName(){
    return name;
}

// workload = tasks that are not finished yet, so it drops when work is completed, rejected or cancelled
int Employee::getWorkload() const{
    int active = 0;
    for (auto* t : tasks) {
        if (!t->getState()->isFinished()) active++;
    }
    return active;
}
void Employee::assignTask(Task* t){
    if (!t) return;
    if (std::find(tasks.begin(), tasks.end(), t) == tasks.end()) tasks.push_back(t);
    t->setAssignee(this);
}

void Employee::unassignTask(Task* t){
    tasks.erase(std::remove(tasks.begin(), tasks.end(), t), tasks.end());
}

void Employee::Operation(){
    if(tasks.empty()){
        std::cout << "No tasks assigned yet" << std::endl;
        return;
    }

    for (auto* c : tasks) {
        std::cout << "Performing task: " << c->getName() << std::endl;
    };
}




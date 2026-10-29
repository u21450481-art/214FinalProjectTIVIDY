#include "Employee.h"

#include <iostream>

Employee::Employee(std::string role, int experience): role(role), workload(0), experience(experience){}

Employee::~Employee(){}

std::string Employee::getRole() const{
    return role;
}

int Employee::getExperience() const{
    return experience;
}
int Employee::getWorkload() const{
    return workload;
}
void Employee::assignTask(Task* t){
    if(t) tasks.push_back(t);
    t->setAssignee(this);
    workload++;
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




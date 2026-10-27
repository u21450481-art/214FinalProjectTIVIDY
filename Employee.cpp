#include "Employee.h"

#include <iostream>

Employee::Employee(std::string name): employeeName(name){}

Employee::~Employee(){}

std::string Employee::getName(){
    return employeeName;
}
void Employee::assignTask(Task* t){
    if(t) tasks.push_back(t);
    t->setAssignee(this);
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




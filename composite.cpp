#include "WorkflowComponent.h"
#include "WorkflowIterator.h"
#include "Subprocess.h"
#include "Task.h"
#include "Participant.h"
#include "Department.h"
#include "Employee.h"
#include "Stage.h"
#include "ConcreteStages.h"

#include <iostream>

int main(){
    std::cout << "=============== Testing Workflow ===============" << std::endl;
    Subprocess* sub1 = new Subprocess();
    Task* t1a = new Task(1,"02/12/2026 09:00","task1a", "p1");
    Task* t1b = new Task(2,"02/12/2026 09:00","task1b","p2");
    Task* t1c = new Task(3,"02/12/2026 09:00","task1c","p3");
    Subprocess* sub2 = new Subprocess();
    Task* t2a = new Task(3,"02/12/2026 09:00","task2a","p4");
    Task* t2b = new Task(3,"02/12/2026 09:00","task2b","p4");
    Task* t2c = new Task(3,"02/12/2026 09:00","task2b","p4");

    sub1->add(t1a);
    sub1->add(t1b);
    sub1->add(t1c);

    sub2->add(t2a);
    sub2->add(t2b);
    sub2->add(t2c);

    sub1->add(sub2);

    WorkflowIterator* iterator = sub1->createIterator();

    WorkflowComponent* a = iterator->currentItem();
    while(!iterator->isDone()){
        std::cout << a->getName() << " ->" << std::endl;
        a = iterator->next();
    }

    delete iterator;
    delete sub1;
}
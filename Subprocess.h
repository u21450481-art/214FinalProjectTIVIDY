#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#include <string>
#include <vector>
#include <memory>

#include "WorkflowComponent.h"
#include "Task.h"

using namespace std;

class Subprocess: public WorkflowComponent {
    std::vector<WorkflowComponent*> children;
    std::string name;

    public:
    Subprocess(std::string name = "Subprocess");
    ~Subprocess();
    void execute();
    std::string getName();
    void add(WorkflowComponent* c) override;
    void remove(WorkflowComponent* c) override;
    std::vector<WorkflowComponent*> getChildren() override;
    bool allChildrenComplete();
    
};




#endif
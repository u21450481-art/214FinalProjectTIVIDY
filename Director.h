// Director.h
#ifndef DIRECTOR_H
#define DIRECTOR_H
#include <string>
#include <vector>
#include "WorkflowDefinitionBuilder.h"

struct EmployeeSpec
{
    std::string name;
    std::string department;
};
struct SubprocessSpec
{
    std::string name;
    std::string parent;
};
struct TaskSpec
{
    std::string name;
    std::string parent;
    int priority;
    std::string branchCondition;
};

class Director
{
    WorkflowDefinitionBuilder *builder; // not owned
public:
    explicit Director(WorkflowDefinitionBuilder *b = nullptr);
    void setBuilder(WorkflowDefinitionBuilder *b);
    void construct(const std::string &name, int version,
                   const std::vector<std::string> &departments,
                   const std::vector<EmployeeSpec> &employees,
                   const std::vector<SubprocessSpec> &subprocesses,
                   const std::vector<TaskSpec> &tasks);
};
#endif
// Director.cpp
#include "Director.h"
#include <stdexcept>

Director::Director(WorkflowDefinitionBuilder *b) : builder(b) {}
void Director::setBuilder(WorkflowDefinitionBuilder *b) { builder = b; }

void Director::construct(const std::string &name, int version,
                         const std::vector<std::string> &departments,
                         const std::vector<EmployeeSpec> &employees,
                         const std::vector<SubprocessSpec> &subprocesses,
                         const std::vector<TaskSpec> &tasks)
{
    if (!builder)
        throw std::logic_error("Director: no builder set.");
    try
    {
        builder->reset(name, version);
        for (const auto &d : departments)
            builder->addDepartment(d);
        for (const auto &e : employees)
            builder->addEmployee(e.name, e.department, e.role, e.experience);
        for (const auto &s : subprocesses)
            builder->addSubprocess(s.name, s.parent);
        for (const auto &t : tasks)
            builder->addTask(t.name, t.parent, t.priority, t.branchCondition, "");
    }
    catch (...)
    {
        delete builder->getResult(); // discard the half-built product
        throw;
    }
}
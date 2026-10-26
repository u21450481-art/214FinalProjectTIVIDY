
class WorkflowEngine
{
private:
    CustomDefinitionBuilder customBuilder;
    std::map<std::string, WorkflowDefinition *> definitions;
    std::map<int, std::unique_ptr<WorkflowInstance>> activeInstances;

public:
    // 1. Facade methods for building ANY industry workflow definition
    void beginDefinition(const std::string &name, int version = 1)
    {
        customBuilder.reset(name, version);
    }
    void addDepartmentToDef(const std::string &deptName)
    {
        customBuilder.addDepartment(deptName);
    }
    void addEmployeeToDef(const std::string &empName, const std::string &deptName = "")
    {
        customBuilder.addEmployee(empName, deptName);
    }
    void addSubprocessToDef(const std::string &subName, const std::string &parent = "")
    {
        customBuilder.addSubprocess(subName, parent);
    }
    void addTaskToDef(const std::string &taskName, const std::string &parent = "",
                      int priority = 1, const std::string &condition = "")
    {
        customBuilder.addTask(taskName, parent, priority, condition);
    }
    void finalizeDefinition()
    {
        WorkflowDefinition *def = customBuilder.getResult();
        if (def != nullptr)
        {
            definitions[def->getName()] = def;
        }
    }

    // 2. Spawn an independent WorkflowInstance from a built definition
    // PROTOTYPE
    int startWorkflow(const std::string &workflowName)
    {
        if (definitions.count(workflowName) == 0)
            return -1;
        std::unique_ptr<WorkflowInstance> inst = definitions[workflowName]->instantiate();
        int id = inst->getId();
        inst->start();
        activeInstances[id] = std::move(inst);
        return id;
    }
};
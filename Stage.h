#ifndef STAGE_H
#define STAGE_H

#include <string>

// Forward declaration of Composite base class
class WorkflowComponent;

/**
 * @brief Abstract State interface representing a workflow lifecycle stage.
*/
class Stage{
public:
    virtual ~Stage() {};

    /**
     * @brief Primary execution method delegated by WorkflowComponent.
     * @param c Pointer to the WorkflowComponent context.
    */
    virtual void handleExecute(WorkflowComponent* c) = 0;

    //Transition triggers
    virtual void start(WorkflowComponent* c);
    virtual void complete(WorkflowComponent* c);
    virtual void reject(WorkflowComponent* c);
    virtual void cancel(WorkflowComponent* c);

    /**
     * @brief Gets the state name.
     */
     virtual std::string getName() const = 0;

    //tells whether this state is final (Complete, Rejected, Cancelled). The default is false
    virtual bool isFinished() const;
};


#endif
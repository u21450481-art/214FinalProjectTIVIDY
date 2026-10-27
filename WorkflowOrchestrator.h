#ifndef WORKFLOWORCHESTRATOR_H
#define WORKFLOWORCHESTRATOR_H

#include "WorkflowEvent.h"

class WorkflowComponent;

/**
 * @brief Mediator interface (GoF role: Mediator).
 *
 * Components hold a pointer to this interface only. They never refer to
 * each other, so a task cannot start another task directly.
 */
class WorkflowOrchestrator {
public:
    virtual ~WorkflowOrchestrator() {}

    /**
     * @brief A colleague reports that something happened to it.
     * @param sender The component the event is about.
     * @param event  What happened.
     */
    virtual void notify(WorkflowComponent* sender, WorkflowEvent event) = 0;
};

#endif

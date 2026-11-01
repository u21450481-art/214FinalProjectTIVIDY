#ifndef WORKFLOWEVENT_H
#define WORKFLOWEVENT_H

/**
 * @brief Things that can happen to a WorkflowComponent that the Mediator cares about.
 *
 * Components report these through WorkflowComponent::notifyMediator().
 * The EngineMediator decides what (if anything) happens next.
 */
enum class WorkflowEvent {
    Started,
    Finished,
    Failed,
    Rejected,
    Cancelled,
    Escalated
};

inline const char* toString(WorkflowEvent e) {
    static const char* const names[] = {
        "Started", "Finished", "Failed", "Rejected", "Cancelled", "Escalated"
    };
    return names[static_cast<int>(e)];
}

#endif

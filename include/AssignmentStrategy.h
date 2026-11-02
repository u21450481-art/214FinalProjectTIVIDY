#ifndef ASSIGNMENTSTRATEGY_H
#define ASSIGNMENTSTRATEGY_H

#include <string>
#include <vector>

#include "Task.h"
#include "Participant.h"

// class Task;
// class Participant;

/**
 * @brief Strategy interface (GoF role: Strategy).
 *
 * Decides WHO should do a task. It only chooses; it never changes the task
 * or the participant. The mediator applies the choice.
 */
class AssignmentStrategy {
public:
    virtual ~AssignmentStrategy() {}

    /**
     * @brief Pick one participant for a task.
     * @param task       The task that needs a performer.
     * @param candidates People who may be chosen (leaf participants).
     * @return The chosen participant, or nullptr if nobody is suitable.
     */
    virtual Participant* assign(const Task& task,
                                const std::vector<Participant*>& candidates) = 0;

    virtual std::string getName() const = 0;
};

#endif

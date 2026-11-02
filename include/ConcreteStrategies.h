#ifndef CONCRETESTRATEGIES_H
#define CONCRETESTRATEGIES_H

#include "AssignmentStrategy.h"

/**
 * Chooses among candidates whose role matches the task's required role.
 * If the task requires no role, everyone matches. Ties go to the person
 * with the lowest workload, then to the first in the list.
 */
class RoleBasedStrategy : public AssignmentStrategy {
    std::size_t nextCandidate = 0;
public:
    Participant* assign(const Task& task,
                        const std::vector<Participant*>& candidates) override;
    std::string getName() const override { return "RoleBased"; }
};

/**
 * Chooses the candidate with the fewest active tasks. Ties rotate through
 * the candidate list to spread sequential work fairly.
 */
class LeastLoadedStrategy : public AssignmentStrategy {
    std::size_t nextCandidate = 0;
public:
    Participant* assign(const Task& task,
                        const std::vector<Participant*>& candidates) override;
    std::string getName() const override { return "LeastLoaded"; }
};

/**
 * Chooses the candidate with the most experience. Ties go to the person
 * with the lowest workload, then to the first in the list.
 */
class MostExperienceStrategy : public AssignmentStrategy {
    std::size_t nextCandidate = 0;
public:
    Participant* assign(const Task& task,
                        const std::vector<Participant*>& candidates) override;
    std::string getName() const override { return "MostExperience"; }
};

#endif

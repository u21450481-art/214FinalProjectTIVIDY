#include "ConcreteStrategies.h"
#include "Task.h"
#include "Participant.h"

//=================RoleBased====================
Participant* RoleBasedStrategy::assign(const Task& task,
                                       const std::vector<Participant*>& candidates) {
    const std::string& needed = task.getRequiredRole();
    Participant* best = nullptr;

    for (auto* p : candidates) {
        if (!p) continue;
        if (!needed.empty() && p->getRole() != needed) continue;
        if (!best || p->getWorkload() < best->getWorkload()) best = p;
    }
    return best;
}

//=================LeastLoaded====================
Participant* LeastLoadedStrategy::assign(const Task& /*task*/,
                                         const std::vector<Participant*>& candidates) {
    Participant* best = nullptr;

    for (auto* p : candidates) {
        if (!p) continue;
        if (!best || p->getWorkload() < best->getWorkload()) best = p;
    }
    return best;
}

//=================MostExperience====================
Participant* MostExperienceStrategy::assign(const Task& /*task*/,
                                            const std::vector<Participant*>& candidates) {
    Participant* best = nullptr;

    for (auto* p : candidates) {
        if (!p) continue;
        if (!best
            || p->getExperience() > best->getExperience()
            || (p->getExperience() == best->getExperience()
                && p->getWorkload() < best->getWorkload())) {
            best = p;
        }
    }
    return best;
}

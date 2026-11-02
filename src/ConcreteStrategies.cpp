#include "ConcreteStrategies.h"
#include "Task.h"
#include "Participant.h"

//=================RoleBased====================
Participant* RoleBasedStrategy::assign(const Task& task,
                                       const std::vector<Participant*>& candidates) {
    const std::string& needed = task.getRequiredRole();
    Participant* best = nullptr;
    std::size_t bestIndex = 0;

    for (std::size_t offset = 0; offset < candidates.size(); ++offset) {
        const std::size_t index = (nextCandidate + offset) % candidates.size();
        Participant* p = candidates[index];
        if (!p) continue;
        if (!needed.empty() && p->getRole() != needed) continue;
        if (!best || p->getWorkload() < best->getWorkload()) {
            best = p;
            bestIndex = index;
        }
    }
    if (best) nextCandidate = (bestIndex + 1) % candidates.size();
    return best;
}

//=================LeastLoaded====================
Participant* LeastLoadedStrategy::assign(const Task& /*task*/,
                                         const std::vector<Participant*>& candidates) {
    Participant* best = nullptr;
    std::size_t bestIndex = 0;

    for (std::size_t offset = 0; offset < candidates.size(); ++offset) {
        const std::size_t index = (nextCandidate + offset) % candidates.size();
        Participant* p = candidates[index];
        if (!p) continue;
        if (!best || p->getWorkload() < best->getWorkload()) {
            best = p;
            bestIndex = index;
        }
    }
    if (best) nextCandidate = (bestIndex + 1) % candidates.size();
    return best;
}

//=================MostExperience====================
Participant* MostExperienceStrategy::assign(const Task& /*task*/,
                                            const std::vector<Participant*>& candidates) {
    Participant* best = nullptr;
    std::size_t bestIndex = 0;

    for (std::size_t offset = 0; offset < candidates.size(); ++offset) {
        const std::size_t index = (nextCandidate + offset) % candidates.size();
        Participant* p = candidates[index];
        if (!p) continue;
        if (!best
            || p->getExperience() > best->getExperience()
            || (p->getExperience() == best->getExperience()
                && p->getWorkload() < best->getWorkload())) {
            best = p;
            bestIndex = index;
        }
    }
    if (best) nextCandidate = (bestIndex + 1) % candidates.size();
    return best;
}

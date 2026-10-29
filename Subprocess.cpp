#include "Subprocess.h"
#include <algorithm>

Subprocess::Subprocess(std::string n) : name(n) {}

std::string Subprocess::getName()
{
    return name;
}

Subprocess::~Subprocess()
{
    for (auto *c : children)
        delete c;
}

void Subprocess::add(WorkflowComponent *c)
{
    if (c)
        children.push_back(c);
}

void Subprocess::remove(WorkflowComponent *c)
{
    children.erase(std::remove(children.begin(), children.end(), c), children.end());
}

std::vector<WorkflowComponent *> Subprocess::getChildren()
{
    return children;
}

void Subprocess::execute()
{
    for (auto *c : children)
        c->execute();
}

bool Subprocess::allChildrenComplete()
{
    for (auto *c : children)
        if (c->getState()->getName() != "Complete")
            return false;
    return true;
}

// Subprocess.cpp
WorkflowComponent *Subprocess::clone() const
{
    Subprocess *copy = new Subprocess(name); // keep the name
    for (WorkflowComponent *child : children)
        copy->add(child->clone()); // recursion clones the whole subtree
    return copy;
}

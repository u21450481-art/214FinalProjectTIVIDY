#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "Subprocess.h"
#include "Task.h"
#include "WorkflowIterator.h"
#include "Department.h"
#include "Employee.h"



static Subprocess* buildTree() {
    Subprocess* sub1 = new Subprocess();
    Subprocess* sub2 = new Subprocess();
    sub1->add(new Task(1, "02/12/2026 09:00", "task1a", "p1"));
    sub1->add(new Task(2, "02/12/2026 09:00", "task1b", "p2"));
    sub1->add(new Task(3, "02/12/2026 09:00", "task1c", "p3"));
    sub2->add(new Task(3, "02/12/2026 09:00", "task2a", "p4"));
    sub2->add(new Task(3, "02/12/2026 09:00", "task2b", "p4"));
    sub2->add(new Task(3, "02/12/2026 09:00", "task2c", "p4"));
    sub1->add(sub2);
    return sub1;
}

TEST(IteratorTest, VisitsNodesDepthFirst) {
    Subprocess* root = buildTree();
    WorkflowIterator* it = root->createIterator();

    std::vector<std::string> visited;
    for (WorkflowComponent* c = it->currentItem(); !it->isDone(); c = it->next())
        visited.push_back(c->getName());

    ASSERT_EQ(visited.size(), 8u);
    EXPECT_EQ(visited[1], "task1a");
    EXPECT_EQ(visited[3], "task1c");
    EXPECT_EQ(visited[5], "task2a");   

    delete it;    
    delete root;   
}

TEST(IteratorTest, LeafIteratorVisitsOnlyItself) {
    Task t(1, "02/12/2026 09:00", "solo", "p1");
    WorkflowIterator* it = t.createIterator();
    EXPECT_FALSE(it->isDone());
    EXPECT_EQ(it->currentItem(), &t);
    EXPECT_EQ(it->next(), nullptr);
    EXPECT_TRUE(it->isDone());
    delete it;
}

TEST(CompositeTest, GetChildrenOfLeafIsEmpty) {
    Task t(1, "02/12/2026 09:00", "solo", "p1");
    EXPECT_TRUE(t.getChildren().empty());
}

TEST(CompositeTest, RemoveDetachesChild) {
    Subprocess sub;
    Task* t = new Task(1, "02/12/2026 09:00", "a", "p1");
    sub.add(t);
    EXPECT_EQ(sub.getChildren().size(), 1u);
    sub.remove(t);
    EXPECT_TRUE(sub.getChildren().empty());
    delete t;  
}

TEST(StateTest, NewComponentIsNotStarted) {
    Task t(1, "02/12/2026 09:00", "a", "p1");
    EXPECT_EQ(t.getState()->getName(), "NotStarted");
}

TEST(StateTest, StartMovesToInProgress) {   
    Task t(1, "02/12/2026 09:00", "a", "p1");
    t.start();
    EXPECT_EQ(t.getState()->getName(), "InProgress");
}

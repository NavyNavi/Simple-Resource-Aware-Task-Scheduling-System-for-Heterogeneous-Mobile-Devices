#include <jni.h>
#include <string>
#include <list>
#include <stdio.h>
#include <android/log.h>
#include "DAG/start-node.cpp"
#include "DAG/task-node.h"
#include "worker.cpp"
#include "result-ticket.cpp"
#include "scheduler.h"

Scheduler::~Scheduler() {
    start_node->cleanup();
    delete start_node;
    start_node = nullptr;
}

TaskNode * Scheduler::addNode(int funcId, TaskNode *prev_node) {
    ALOG("Add node.");
    TaskNode* new_node = new TaskNode(funcId, 1, idCounter);
    idCounter++;
    if (prev_node) {
        prev_node->addNextNode(new_node);
    } else {
        start_node->addNextNode(new_node);
    }
    return new_node;
}

std::string Scheduler::commit() {
    currCrit = start_node->setCriticality();
    std::string task = start_node->getNextNode()->serialize();
    return task;
}

void Scheduler::assign() {
    //temp create worker here
    Worker worker1;
    Worker worker2;
    worker1.receiveTask(*start_node->getNextNode());
}

void Scheduler::commitNode(int id) {
    std::list<TaskNode*>::iterator it;
    for (it = pending_node.begin(); it != pending_node.end(); ++ it) {
        if ((*it)->id == id) {
            std::vector<TaskNode*> ready_node = (*it)->commit();
        }
    }
}
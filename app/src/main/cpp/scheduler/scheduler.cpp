#include <jni.h>
#include <string>
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

TaskNode * Scheduler::addNode(callback_function func, TaskNode *prev_node) {
    ALOG("Add node.");
    TaskNode* new_node = new TaskNode(reinterpret_cast<int (*)(int)>(func), 1, idCounter);
    idCounter++;
    if (prev_node) {
        prev_node->addNextNode(new_node);
    } else {
        start_node->addNextNode(new_node);
    }
    return new_node;
}

void Scheduler::commit() {
    currCrit = start_node->setCriticality();
    this->assign();
}

void Scheduler::assign() {
    //temp create worker here
    Worker worker1;
    Worker worker2;
    worker1.receiveTask(*start_node->getNextNode());
}
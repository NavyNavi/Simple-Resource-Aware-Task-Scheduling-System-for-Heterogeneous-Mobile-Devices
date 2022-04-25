#include <jni.h>
#include <string>
#include <stdio.h>
#include <future>
#include <cmath>
#include <android/log.h>
#include "node.h"
#include "task-node.h"

TaskNode::TaskNode(callback_function f, int args, long id) {
    this->func = f;
    this->args = args;
    this->id = id;
}

TaskNode::TaskNode(TaskNode const &node) {
    this->id = node.id;
    this->criticality = node.getCriticality();
    this->next_nodes = node.next_nodes;
    this->func = node.func;
    this->args = node.args;
}

TaskNode * TaskNode::getNextNode() { return next_nodes.front(); }

void TaskNode::addNextNode(TaskNode *node) { next_nodes.push_back(node); }

int TaskNode::execute() {
    std::packaged_task<int()> task(std::bind(func, args));
    std::future<int> future = task.get_future();
    task();
    ALOG("Task node execution.");
    return future.get();
}

int TaskNode::setCriticality(){
            if((criticality) == 0) {
                int max = 0;
                for (TaskNode* edge : next_nodes) {
                    int new_max = edge->setCriticality();
                    max = new_max > max ? new_max : max;
                }
                criticality = ++max;
            }
            return criticality;
        }

void TaskNode::cleanup(){
    if(next_nodes.empty()) { return; }
    for (TaskNode *edge : next_nodes) {
        edge->cleanup();
        delete edge;
        edge = nullptr;
    }
}
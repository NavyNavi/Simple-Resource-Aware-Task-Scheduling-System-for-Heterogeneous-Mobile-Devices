#include <jni.h>
#include <string>
#include <sstream>
#include <stdio.h>
#include <future>
#include <cmath>
#include <android/log.h>
#include "node.h"
#include "task-node.h"

TaskNode::TaskNode(int funcId, int args, long id) {
    this->funcId = funcId;
    this->args = args;
    this->id = id;
}

TaskNode::TaskNode(TaskNode const &node) {
    this->id = node.id;
    this->criticality = node.getCriticality();
    this->next_nodes = node.next_nodes;
    this->funcId = node.funcId;
    this->args = node.args;
}

TaskNode * TaskNode::getNextNode() { return next_nodes.front(); }

void TaskNode::addNextNode(TaskNode *node) { next_nodes.push_back(node); }

int TaskNode::execute() {
    //std::packaged_task<int()> task(std::bind(funcId, args));
    //std::future<int> future = task.get_future();
    //task();
    ALOG("Task node execution.");
    //return future.get();
    return 1;
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

void TaskNode::findCriticalNodes() {
    if(critical) {
        for (TaskNode* edge : next_nodes) {
            if (edge->id == (this->id - 1)) {
                edge->critical = true;
                edge->findCriticalNodes();
            }
        }
    }
}


void TaskNode::cleanup(){
    if(next_nodes.empty()) { return; }
    for (TaskNode *edge : next_nodes) {
        edge->cleanup();
        delete edge;
        edge = nullptr;
    }
}

std::list<TaskNode*> TaskNode::commit() {
    std::list<TaskNode*> ready_node;
    if(next_nodes.empty()) { return ready_node; }
    for (TaskNode *edge : next_nodes) {
        edge->dependency--;
        if (edge->dependency <= 0) {
            ready_node.push_back(edge);
        }
    }
    ready_node.sort();
    return ready_node;
}

std::string TaskNode::serialize() {
    ALOG("Task node: serializing.");
    std::ostringstream ss;
    //finished || node id || function id || arguments || start time
    ss << "0 " << id << " " << funcId << " " << args; //<< start_time;
    return ss.str();
}
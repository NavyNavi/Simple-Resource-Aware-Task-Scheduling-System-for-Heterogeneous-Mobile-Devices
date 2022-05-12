#include <jni.h>
#include <string>
#include <stdio.h>
#include "node.h"
#include <android/log.h>
#include "task-node.h"
#include "start-node.h"

TaskNode * StartNode::getNextNode() { return next_nodes.front(); }

void StartNode::addNextNode(TaskNode *node) { next_nodes.push_back(node); }

int StartNode::setCriticality() {
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

void StartNode::findCriticalNodes() {
    if(critical) {
        for (TaskNode* edge : next_nodes) {
            if (edge->id == (this->id - 1)) {
                edge->critical = true;
                edge->findCriticalNodes();
            }
        }
    }
}

int StartNode::execute() { return 0; }

void StartNode::cleanup() {
    if(next_nodes.empty()) { return; }
    for (TaskNode *edge : next_nodes) {
        edge->cleanup();
        delete edge;
        edge = nullptr;
    }
}

std::list<TaskNode*> StartNode::commit() {
    ALOG("start node: finding next nodes.");
    std::list<TaskNode*> ready_node;
    if(next_nodes.empty()) { return ready_node; }
    for (TaskNode *edge : next_nodes) {
        edge->dependency--;
        if (edge->dependency == 0) {
            ready_node.push_back(edge);
        }
    }
    return ready_node;
}

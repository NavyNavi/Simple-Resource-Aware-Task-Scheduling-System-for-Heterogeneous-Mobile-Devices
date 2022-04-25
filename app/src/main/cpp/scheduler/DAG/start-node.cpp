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

int StartNode::execute() { return 0; }

void StartNode::cleanup() {
    if(next_nodes.empty()) { return; }
    for (TaskNode *edge : next_nodes) {
        edge->cleanup();
        delete edge;
        edge = nullptr;
    }
}
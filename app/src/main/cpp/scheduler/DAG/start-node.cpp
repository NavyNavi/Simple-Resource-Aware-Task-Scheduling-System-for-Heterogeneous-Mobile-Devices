#include <jni.h>
#include <string>
#include <stdio.h>
#include "node.cpp"
#include "task-node.cpp"

class StartNode : public Node {
    public:
        TaskNode* getNextNode() { return next_nodes.front(); }
        void addNextNode(TaskNode* node) { next_nodes.push_back(node); }

        int execute() { return 0; }
        int setCriticality() {
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
        void cleanup() {
            if(next_nodes.empty()) { return; }
            for (TaskNode *edge : next_nodes) {
                edge->cleanup();
                delete edge;
                edge = nullptr;
            }
        }
    private:
        std::vector<TaskNode *> next_nodes;
};
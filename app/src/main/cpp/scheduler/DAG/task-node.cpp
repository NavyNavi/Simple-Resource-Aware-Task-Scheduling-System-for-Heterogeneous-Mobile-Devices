#ifndef TASKNODE_INCLUDED
#define TASKNODE_INCLUDED

#include <jni.h>
#include <string>
#include <stdio.h>
#include <future>
#include <cmath>
#include "node.cpp"


class TaskNode : public Node {
    public:

        typedef int (*callback_function)(int);
        TaskNode(callback_function f, int args, long id) : t(std::bind(f, 12)) {
            this->id = id;
        }
        /*TaskNode(TaskNode node) {
            this->id = node.id;
            this->criticality = node.getCriticality();
            this->t =
        }*/

        TaskNode* getNextNode() { return next_nodes.front(); }
        void addNextNode(TaskNode* node) { next_nodes.push_back(node); }

        int execute() {
            t();
            return future.get();
        }

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
        std::packaged_task<int()> t;
        std::future<int> future = t.get_future(); //really need?
};

#endif
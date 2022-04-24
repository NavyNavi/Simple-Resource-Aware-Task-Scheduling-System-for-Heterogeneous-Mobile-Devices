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
        TaskNode(callback_function f, int args, long id) {
            this->func = f;
            this->args = args;
            this->id = id;
        }
        TaskNode(TaskNode const &node) {
            this->id = node.id;
            this->criticality = node.getCriticality();
            this->next_nodes = node.next_nodes;
            this->func = node.func;
            this->args = node.args;
        }

        TaskNode* getNextNode() { return next_nodes.front(); }
        void addNextNode(TaskNode* node) { next_nodes.push_back(node); }

        int execute() {
            std::packaged_task<int()> task(std::bind(func, args));
            std::future<int> future = task.get_future();
            task();
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
        callback_function func;
        int args;
};

#endif
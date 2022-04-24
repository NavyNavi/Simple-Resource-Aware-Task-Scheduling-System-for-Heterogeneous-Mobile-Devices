#include <jni.h>
#include <string>
#include <stdio.h>
#include <android/log.h>
#include "DAG/start-node.cpp"
#include "DAG/task-node.cpp"
#include "worker.cpp"
#include "result-ticket.cpp"

class Scheduler {
    public:
        ~Scheduler() {
            start_node->cleanup();
            delete start_node;
            start_node = nullptr;
        }

        typedef int (*callback_function)(void);
        TaskNode* addNode(callback_function func, TaskNode* prev_node = nullptr) {
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
        //commitToScheduler -> find criticality, form queue, start assign
        void commit() {
            currCrit = start_node->setCriticality();
            this->assign();
        }

        void assign() {
            //temp create worker here
            Worker worker1;
            Worker worker2;
            worker1.receiveTask(*start_node->getNextNode());
        }




        //on end
    private:
        int currCrit;
        long idCounter = 1;
        StartNode* start_node = new StartNode();
};
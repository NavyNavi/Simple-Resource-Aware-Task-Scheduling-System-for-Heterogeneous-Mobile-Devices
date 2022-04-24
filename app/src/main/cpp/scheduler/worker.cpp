#include <jni.h>
#include <string>
#include <thread>
#include <queue>
#include <stdio.h>
#include <android/log.h>
#include "DAG/task-node.cpp"
#include "result-container.cpp"

class Worker {
public:
    void receiveTask(TaskNode task) {
        task_queue.push(task);
        if (task_queue.size() == 1) {
            std::thread tExecutor(&Worker::exeTasks, this);
            tExecutor.join();
        }
    }
    void exeTasks() {
        while (!task_queue.empty()){
            TaskNode* task = &task_queue.front();
            int res = task->execute();
            ALOG("Worker execution result : %d.", res);
            new ResultContainer(res, task->id);
            //send finish noti to scheduler
            task_queue.pop();
        }
    }
private:
    //int num_threads = std::thread::hardware_concurrency();
    std::queue<TaskNode> task_queue;
};
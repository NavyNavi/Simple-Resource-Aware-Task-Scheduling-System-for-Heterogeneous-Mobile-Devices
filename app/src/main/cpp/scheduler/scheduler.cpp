#include <jni.h>
#include <string>
#include <list>
#include <stdio.h>
#include <android/log.h>
#include "DAG/start-node.cpp"
#include "DAG/task-node.h"
#include "worker.cpp"
#include "result-ticket.cpp"
#include "scheduler.h"
#include "profiler.cpp"

Scheduler::~Scheduler() {
    ALOG("scheduler: dtor.");
    id2address.clear();
    start_node->cleanup();
    delete start_node;
    start_node = nullptr;
}

TaskNode * Scheduler::addNode(int funcId, TaskNode *prev_node) {
    ALOG("Add node.");
    TaskNode* new_node = new TaskNode(funcId, 1, idCounter);
    id2address[idCounter] = new_node;
    idCounter++;
    if (prev_node) {
        prev_node->addNextNode(new_node);
    } else {
        start_node->addNextNode(new_node);
    }
    return new_node;
}

std::string Scheduler::startScheduler(JNIEnv* env, jobject jObj) {
    ALOG("scheduler: initializing.");
    this->env = env;
    this->jObj = jObj;
    currCrit = start_node->setCriticality();
    std::list<TaskNode*> ready_node = start_node->commit();
    pending_node.merge(ready_node);
    assign();
}

void Scheduler::assign() {
    ALOG("scheduler: assigning task.");
    assigning = true;
    while(assigning){
        if(pending_node.empty()) {
            ALOG("scheduler: all task assigned.");
            assigning = false;
            break;
        }

        //add thread safety
        Profiler& profiler = Profiler::getInstance();
        TaskNode* node = pending_node.front();

        int worker;
        if(node->getCriticality() >= currCrit) {
            currCrit--;
            worker = profiler.getBestWorker();
        } else {
            worker = profiler.getWorker();
        }
        ALOG("scheduler: got worker %d", worker);

        std::string task = node->serialize();
        profiler.useWorker(worker);
        //timeout for recovery
        sendTask(task, worker);
        pending_node.pop_front();
    }
}

void Scheduler::sendTask(std::string task, int worker) {
    ALOG("scheduler: sending task of length %d", task.length());
    ALOG("scheduler: env is %p", env);
    jstring jTask = env->NewStringUTF(((std::string)"a").c_str());
    jclass taskClass = env->FindClass("com/example/myapplication/MainActivity");
    jmethodID methodId = env->GetMethodID(taskClass, "setSerializedTask", "(Ljava/lang/String;I)V");
    env->CallVoidMethod(jObj, methodId, jTask, worker);
}

void Scheduler::commitNode(int id) {
    ALOG("scheduler: node completed.");
    std::list<TaskNode*> ready_node = id2address[id]->commit();
    pending_node.merge(ready_node);
    if(!assigning) { assign(); }
}
#include <jni.h>
#include <string>
#include <list>
#include <stdio.h>
#include <android/log.h>
#include "DAG/start-node.cpp"
#include "DAG/task-node.h"
#include "worker.cpp"
#include "scheduler.h"
#include "profiler.h"

Scheduler::~Scheduler() {
    ALOG("scheduler: dtor.");
    id2address.clear();
    start_node->cleanup();
    delete start_node;
    start_node = nullptr;
}

TaskNode * Scheduler::addNode(int funcId, TaskNode *prev_node) {
    TaskNode* new_node = new TaskNode(funcId, 1, idCounter);
    id2address[idCounter] = new_node;
    ALOG("Add node %d: %p.", idCounter, new_node);
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
    updateEnv(env, jObj);
    currCrit = start_node->setCriticality();
    start_node->findCriticalNodes();
    std::list<TaskNode*> ready_node = start_node->commit();
    pending_node.merge(ready_node);
    assign();
    return "";
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
        if(node->getCriticality()) {
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
    jstring jTask = env->NewStringUTF((task).c_str());
    jclass taskClass = env->FindClass("com/example/myapplication/WiFiDirect");
    jmethodID methodId = env->GetMethodID(taskClass, "sendSerializedTask", "(Ljava/lang/String;I)V");
    env->CallVoidMethod(jObj, methodId, jTask, worker);
}

void Scheduler::commitNode(int id) {
    ALOG("scheduler: node completed %d.", id);
    std::list<TaskNode*> ready_node = id2address[id]->commit();
    if(!ready_node.empty()) {
        for (auto node : ready_node) {
            pending_node.push_back(node);
        }
    }
    if(!assigning) { assign(); }
}

void Scheduler::updateEnv(JNIEnv *env, jobject jObj) {
    this->env = env;
    this->jObj = jObj;
}
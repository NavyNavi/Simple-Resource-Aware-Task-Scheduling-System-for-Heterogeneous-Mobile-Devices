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

Scheduler::~Scheduler() {
    start_node->cleanup();
    delete start_node;
    start_node = nullptr;
}

TaskNode * Scheduler::addNode(int funcId, TaskNode *prev_node) {
    ALOG("Add node.");
    TaskNode* new_node = new TaskNode(funcId, 1, idCounter);
    idCounter++;
    if (prev_node) {
        prev_node->addNextNode(new_node);
    } else {
        start_node->addNextNode(new_node);
    }
    return new_node;
}

std::string Scheduler::startScheduler(JNIEnv* env, jobject jObj) {
    this->env = env;
    this->jObj = jObj;
    currCrit = start_node->setCriticality();
    commitNode(start_node->id);
}

void Scheduler::assign() {
    assigning = true;
    while(assigning){
        if(pending_node.empty()) {
            assigning = false;
            break;
        }
        //add thread safety
        //consider criticality
        std::string task = pending_node.front()->serialize();
        sendTask(task);
        pending_node.pop_front();
    }
}

void Scheduler::sendTask(std::string task) {
    jstring jTask = env->NewStringUTF(task.c_str());
    jclass taskClass = env->FindClass("com/example/myapplication/MainActivity");
    jmethodID methodId = env->GetMethodID(taskClass, "setSerializedTask", "(Ljava/lang/String;)V");
    env->CallVoidMethod(jObj, methodId, jTask);
}

void Scheduler::commitNode(int id) {
    std::list<TaskNode*>::iterator it;
    for (it = offloaded_node.begin(); it != offloaded_node.end(); ++ it) {
        if ((*it)->id == id) {
            std::list<TaskNode*> ready_node = (*it)->commit();
            pending_node.merge(ready_node);
            if(!assigning) { assign(); }
        }
    }
}
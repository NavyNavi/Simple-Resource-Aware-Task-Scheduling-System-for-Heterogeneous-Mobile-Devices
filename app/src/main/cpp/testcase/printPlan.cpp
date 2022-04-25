#define  LOG_TAG    "testjni"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#include <jni.h>
#include <string>
#include <stdio.h>
#include "../scheduler/scheduler.cpp"
#include "../scheduler/DAG/task-node.h"

//define tasks
int printLine() {
    return 2;
}

//design DAG
std::string getSolution() {
    ALOG("start test: print plan.");
    Scheduler scheduler;
    TaskNode* node1_1 = scheduler.addNode(printLine);
    TaskNode* node1_2 = scheduler.addNode(printLine);
    TaskNode* node1_3 = scheduler.addNode(printLine);
    TaskNode* node1_4 = scheduler.addNode(printLine);
    TaskNode* node1_1_1 = scheduler.addNode(printLine, node1_1);
    TaskNode* node1_1_2 = scheduler.addNode(printLine, node1_1);
    TaskNode* node1_3_1 = scheduler.addNode(printLine, node1_3);
    TaskNode* node1_1_2_1 = scheduler.addNode(printLine, node1_1_2);
    scheduler.commit();
    return "done";
}
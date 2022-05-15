#define  LOG_TAG    "testjni"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#include <map>
#include <jni.h>
#include <string>
#include <stdio.h>
#include "../scheduler/scheduler.cpp"
#include "../scheduler/worker.h"
#include "../scheduler/DAG/task-node.h"
using namespace std;

//define tasks
int printLine() {
    return 2;
}

typedef int (*callback_function)(void);
std::map<int,callback_function> funcMap = { {0, printLine} };

//design DAG
string getSolution() {
    ALOG("start test: print plan.");

    Scheduler& scheduler = Scheduler::getInstance();
    TaskNode* node1_1 = scheduler.addNode(0);
    //TaskNode* node1_2 = scheduler.addNode(0);
    //TaskNode* node1_3 = scheduler.addNode(0);
    //TaskNode* node1_4 = scheduler.addNode(0);
    TaskNode* node1_1_1 = scheduler.addNode(0, node1_1);
    TaskNode* node1_1_2 = scheduler.addNode(0, node1_1);
    //TaskNode* node1_3_1 = scheduler.addNode(0, node1_3);
    TaskNode* node1_1_2_1 = scheduler.addNode(0, node1_1_2);
    return "started";
}
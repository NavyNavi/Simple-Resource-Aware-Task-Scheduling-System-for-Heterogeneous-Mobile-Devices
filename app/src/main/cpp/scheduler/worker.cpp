#include <jni.h>
#include <string>
#include <thread>
#include <queue>
#include <stdio.h>
#include <android/log.h>
#include "DAG/task-node.cpp"
#include "scheduler.h"
#include "worker.h"

void Worker::setEnv(JNIEnv *env, jobject jObj) {
    this->env = env;
    this->jObj = jObj;
}

void Worker::queueTask(std::string task) {
    ALOG("Worker queue task.");
    std::future<void> future = std::async(std::launch::async, &Worker::exeTasks, this, task);
}

void Worker::exeTasks(std::string task) {
    std::vector<std::string> tokens;
    std::stringstream strstr(task);
    std::copy( std::istream_iterator<std::string>(strstr),
               std::istream_iterator<std::string>(),
               std::back_inserter(tokens) );

    int func_id = std::stoi(tokens[1]);
    int args = std::stoi(tokens[2]);
    std::string start_time = tokens[3];

    int (*fun_ptr)() = funcMap[func_id];
    int res = fun_ptr();
    std::string serialized_res = tokens[0] + std::to_string(worker_id) + std::to_string(res);
    ALOG("Worker execution result : %d.", res);

    jstring jRes = env->NewStringUTF((serialized_res).c_str());
    jclass taskClass = env->FindClass("com/example/myapplication/MainActivity");
    jmethodID methodId = env->GetMethodID(taskClass, "sendResult", "(Ljava/lang/String;)V");
    env->CallVoidMethod(jObj, methodId, jRes);
}
#include "profiler.h"

int Profiler::addWorker() {
    int newId = worker_performance.size();
    worker_performance.insert({0.01, newId});
    ALOG("added worker %f: %d", 0.01 ,newId);
    return newId;
}

void Profiler::updateData(std::string res_str) {
    std::vector<std::string> tokens;
    std::stringstream strstr(res_str);
    std::copy( std::istream_iterator<std::string>(strstr),
               std::istream_iterator<std::string>(),
               std::back_inserter(tokens) );

    float nodeId = std::stof(tokens[0]);
    int workerId = std::stod(tokens[1]);
    int res = std::stod(tokens[2]);

    float key;
    for (auto it : worker_performance) {
        if (it.second == workerId) {
            key = it.first;
            break;
        }
    }
    worker_performance.erase(key);
    worker_performance.insert({nodeId, workerId});

    Scheduler& scheduler = Scheduler::getInstance();
    scheduler.commitNode(nodeId);
}

int Profiler::getBestWorker() {
    ALOG("profiler: getting best worker.");
    if (!worker_performance.empty()) {
        return worker_performance.begin()->second;
    }
    ALOG("profiler: no worker found.");
    return -1;
}

int Profiler::getWorker() {
    ALOG("profiler: getting worker.");
    if (worker_performance.size() > 1) {
        std::multimap<float, int>::iterator it = worker_performance.begin();
        it++;
        return it->second;
    }
    return getBestWorker();
}

void Profiler::useWorker(int worker_id) {
    std::multimap<float, int> ::iterator it;
    for (it = worker_performance.begin(); it != worker_performance.end(); it++)
    {
        if (it->second == worker_id) {
            float duration = it->first;
            worker_performance.erase(it);
            //more accurately only double exec time and not transmit time
            worker_performance.insert({duration*2, worker_id});
            break;
        }
    }
}
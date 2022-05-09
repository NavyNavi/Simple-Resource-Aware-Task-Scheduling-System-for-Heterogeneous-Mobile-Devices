#ifndef PROFILER_INCLUDED
#define PROFILER_INCLUDED

#include "scheduler.h"

class Profiler {
public:
    static Profiler& getInstance()
    {
        static Profiler instance;
        return instance;
    }
    Profiler(Profiler const&)        = delete;
    void operator=(Profiler const&)  = delete;

    void updateData(std::string res_str){
        std::vector<std::string> tokens;
        std::stringstream strstr(res_str);
        std::copy( std::istream_iterator<std::string>(strstr),
                   std::istream_iterator<std::string>(),
                   std::back_inserter(tokens) );

        float duration = std::stof(tokens[0]);
        int workerId = std::stod(tokens[1]);
        int res = std::stod(tokens[2]);

        worker_performance.erase(worker_performance.find(workerId));
        worker_performance.insert({duration, workerId});

        Scheduler& scheduler = Scheduler::getInstance();
        scheduler.commitNode(workerId);
    }

    int getBestWorker() {
        return worker_performance.begin()->second;
    }

    int getWorker() {
        std::multimap<float, int>::iterator it = worker_performance.begin();
        it++;
        return it->second;
    }

    void useWorker(int worker_id) {
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

private:
    Profiler() {}
    //change to map and avoid collision
    std::multimap<float, int> worker_performance;
};

#endif
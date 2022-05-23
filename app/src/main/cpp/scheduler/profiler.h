#ifndef PROFILER_INCLUDED
#define PROFILER_INCLUDED

class Profiler {
public:
    static Profiler& getInstance()
    {
        static Profiler instance;
        return instance;
    }
    Profiler(Profiler const&)        = delete;
    void operator=(Profiler const&)  = delete;

    //change workerId to hash of phone
    int addWorker();
    void updateData(std::string res_str);
    int getBestWorker();
    int getWorker();
    void useWorker(int worker_id);

private:
    Profiler() {}
    std::multimap<float, int> worker_performance;
};

#endif
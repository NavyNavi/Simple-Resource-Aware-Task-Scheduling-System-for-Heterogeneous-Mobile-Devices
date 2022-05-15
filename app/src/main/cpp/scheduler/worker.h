#ifndef WORKER_INCLUDED
#define WORKER_INCLUDED

typedef int (*callback_function)(void);
extern std::map<int,callback_function> funcMap;

class Worker {
public:
    static Worker& getInstance()
    {
        static Worker instance;
        return instance;
    }
    Worker(Worker const&)       = delete;
    void operator=(Worker const&)  = delete;

    void setEnv(JNIEnv* env, jobject jObj);
    void queueTask(std::string task);
    void exeTasks(std::string task);
private:
    Worker() {}
    ~Worker() {}
    int worker_id;
    JNIEnv* env;
    jobject jObj;
};

#endif
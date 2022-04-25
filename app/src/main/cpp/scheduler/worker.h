class Worker {
public:
    void receiveTask(TaskNode task);
    void exeTasks();
private:
    //int num_threads = std::thread::hardware_concurrency();
    std::queue<TaskNode> task_queue;
};
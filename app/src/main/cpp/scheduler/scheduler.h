class Scheduler {
    public:
        static Scheduler& getInstance()
        {
            static Scheduler instance;
            return instance;
        }
        Scheduler(Scheduler const&)       = delete;
        void operator=(Scheduler const&)  = delete;

        TaskNode* addNode(int funcId, TaskNode* prev_node = nullptr);
        //how to make sure its started?
        std::string startScheduler(JNIEnv* env, jobject jObj);
        void commitNode(int id);

    private:
        Scheduler() {}
        ~Scheduler();
        void assign();
        void sendTask(std::string task, int worker);

        int currCrit;
        long idCounter = 1;
        bool assigning = false;
        std::list<TaskNode*> offloaded_node;
        std::list<TaskNode*> pending_node;
        JNIEnv* env;
        jobject jObj;
        StartNode* start_node = new StartNode();
};
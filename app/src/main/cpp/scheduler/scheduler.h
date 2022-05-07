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
        std::string commit();
        void assign();
        std::vector<TaskNode*> commitNode(int id);

    private:
        Scheduler() {}
        ~Scheduler();

        int currCrit;
        long idCounter = 1;
        //sent
        std::list<TaskNode*> pending_node;
        StartNode* start_node = new StartNode();
};